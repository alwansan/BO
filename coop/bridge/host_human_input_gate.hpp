// BO-COOP HUMAN-INPUT-001: host-authoritative value-only ingress contract.
// NO BLACK gameplay integration, guest calls, actor writes or wire trust.
// A producer MUST first authenticate the source through an independent room
// transport and use a verified session/roster membership. All game callbacks
// remain on the single bound game thread only.
// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>

namespace bo_bridge {
struct HumanWorldScope {
    uint64_t session=0;        // Host-authenticated session epoch; never a PIN
    uint32_t scene=0;         // Verified BLACK engine scene epoch, NOT world pointer
    uint32_t roster_revision=0; // Changes on every membership revision
    bool operator==(const HumanWorldScope&) const = default;
};
struct HumanInput {
    HumanWorldScope scope{};
    uint64_t peer_generation=0; // Unique from authoritative host on every join
    uint64_t received_ms=0;     // Monotonic host clock, no wall-clock or PII
    uint32_t sequence=0;        // Per peer generation
    uint8_t player=0;          // 2..4 ONLY. Host player 1 stays original FPS
    int16_t move_x=0,move_y=0; // normalized [-1000,1000] NOT BLACK coordinates
    int16_t yaw=0;             // [-18000,+18000] centidegrees
    uint8_t fire=0,jump=0;     // values 0/1 only; not a weapon implementation
};
static_assert(std::is_trivially_copyable_v<HumanInput>);
inline bool ValidHumanInput(const HumanInput& f) noexcept {
    return f.scope.session!=0 && f.scope.scene!=0 && f.scope.roster_revision!=0 &&
           f.player>=2 && f.player<=4 &&
           f.peer_generation!=0 && f.sequence!=0 &&
           f.move_x>=-1000 && f.move_x<=1000 &&
           f.move_y>=-1000 && f.move_y<=1000 &&
           f.yaw>=-18000 && f.yaw<=18000 &&
           f.fire<=1 && f.jump<=1;
}

class HostHumanInputGate final {
    static constexpr std::size_t kCapacity=48;
    static constexpr std::size_t kMaxDrain=12;
    static constexpr uint64_t kMaxAgeMs=250;
    struct Peer { uint64_t generation=0; uint32_t last_sequence=0; };
    mutable std::mutex mutex_;
    std::array<HumanInput,kCapacity> queue_{};
    std::array<Peer,5> peer_{};
    HumanWorldScope scope_{};
    std::thread::id owner_{};
    std::size_t head_=0,count_=0;
    bool bound_=false;

    void purgePeer(uint8_t player) noexcept {
        std::array<HumanInput,kCapacity> remaining{};
        std::size_t count=0;
        for(std::size_t i=0;i<count_;++i) {
            const HumanInput& f=queue_[(head_+i)%kCapacity];
            if(f.player!=player)remaining[count++]=f;
        }
        queue_=remaining;head_=0;count_=count;
    }
    bool gameThread() const noexcept {
        return bound_ && owner_==std::this_thread::get_id();
    }
public:
    // Only an independently VERIFIED engine game-thread/host scene may bind.
    // Changing session, actual scene epoch or authenticated roster invalidates
    // ALL peer generations and pending commands, not merely sequence numbers.
    bool Bind(const HumanWorldScope& value) noexcept {
        if(!value.session||!value.scene||!value.roster_revision)return false;
        std::lock_guard<std::mutex> lock(mutex_);
        if(bound_ && owner_!=std::this_thread::get_id())return false;
        if(!bound_){owner_=std::this_thread::get_id();bound_=true;}
        if(!(value==scope_)) {
            scope_=value;peer_={};queue_={};head_=count_=0;
        }
        return true;
    }
    // Host-only membership update after authenticated TLS roster validation.
    // Each leave/rejoin must issue a NEW, nonzero generation. If a caller reuses
    // an old generation this cannot cryptographically distinguish sessions.
    bool SetPeer(uint8_t player,uint64_t generation) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        if(!gameThread()||player<2||player>4)return false;
        Peer& peer=peer_[player];
        if(peer.generation==generation)return true;
        peer={generation,0};purgePeer(player);
        return true;
    }
    // Called on a transport worker AFTER source authentication and ticket
    // validation. A boolean alone is NOT authentication: production must
    // implement and enforce it before OfferTrusted.
    bool OfferTrusted(const HumanInput& f,bool source_authenticated) noexcept {
        if(!source_authenticated||!ValidHumanInput(f))return false;
        std::unique_lock<std::mutex> lock(mutex_,std::try_to_lock);
        if(!lock.owns_lock()||!bound_||!(f.scope==scope_)||
           peer_[f.player].generation!=f.peer_generation||
           count_==kCapacity)return false;
        queue_[(head_+count_)%kCapacity]=f;
        ++count_;
        return true;
    }
    template<class Consumer>
    std::size_t Drain(uint64_t now,Consumer&& consumer) {
        std::array<HumanInput,kMaxDrain> batch{};
        std::size_t n=0;
        {
            std::unique_lock<std::mutex> lock(mutex_,std::try_to_lock);
            if(!lock.owns_lock()||!gameThread())return 0;
            std::size_t inspected=0;
            while(count_ && inspected<kMaxDrain) {
                HumanInput f=queue_[head_];
                head_=(head_+1)%kCapacity;--count_;++inspected;
                Peer& p=peer_[f.player];
                if(!(f.scope==scope_)||!p.generation||
                   p.generation!=f.peer_generation||f.sequence<=p.last_sequence||
                   now<f.received_ms||now-f.received_ms>kMaxAgeMs)continue;
                p.last_sequence=f.sequence;
                batch[n++]=f;
            }
        }
        // No guest memory, JNI, or game callback under the transport lock.
        // Re-check before EACH callback because a consumer can synchronously
        // change the scene or remove a peer. Never dispatch later stale batch
        // entries after a reentrant Bind/SetPeer from the owning game thread.
        std::size_t delivered=0;
        for(std::size_t i=0;i<n;++i) {
            const HumanInput& f=batch[i];
            {
                std::unique_lock<std::mutex> lock(mutex_,std::try_to_lock);
                if(!lock.owns_lock()||!gameThread()||
                   !(f.scope==scope_)||peer_[f.player].generation!=f.peer_generation)
                    continue;
            }
            consumer(f);
            ++delivered;
        }
        // Future actor mutation also needs independently verified actor lifetime.
        return delivered;
    }
    std::size_t Pending() const noexcept {
        std::unique_lock<std::mutex> lock(mutex_,std::try_to_lock);
        return lock.owns_lock()?count_:kCapacity;
    }
    static constexpr std::size_t Capacity() noexcept {return kCapacity;}
    static constexpr std::size_t MaxDrain() noexcept {return kMaxDrain;}
};
} // namespace bo_bridge
