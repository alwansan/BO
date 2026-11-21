// BO-COOP isolated NET-004 preparation: no BLACK memory access, no networking.
// This validates a handoff between threads only. NOT a playable multiplayer mod.
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
enum class Kind : uint8_t { RemoteInput=1, PeerLeave=2 };

// Only plain values cross the transport -> game-thread boundary.
// No pointers, guest addresses, object references, Java/JNI or game handles.
struct Command {
    Kind kind=Kind::RemoteInput;
    uint8_t player=0;      // 2, 3, 4; 1 is the authoritative host.
    uint32_t scene=0;      // caller must set an authoritative scene epoch.
    uint32_t sequence=0;   // monotonic within a single scene and player.
    int16_t move_x=0;      // normalized [-1000,+1000]; NOT world coordinates.
    int16_t move_y=0;
};
static_assert(std::is_trivially_copyable_v<Command>);

inline bool Valid(const Command& c) noexcept {
    if(c.player<2 || c.player>4 || !c.scene || !c.sequence) return false;
    if(c.kind==Kind::RemoteInput)
        return c.move_x>=-1000 && c.move_x<=1000 &&
               c.move_y>=-1000 && c.move_y<=1000;
    if(c.kind==Kind::PeerLeave) return !c.move_x && !c.move_y;
    return false;
}

// Bounded, game-neutral thread handoff. Producer never invokes game functions.
// Consumer must BindScene from its actual game-thread entry. Any future code
// applying commands to BLACK needs independent thread and actor-lifetime proof.
class SceneInbox {
    static constexpr std::size_t kCapacity=64;
    static constexpr std::size_t kMaxDrain=16;
    mutable std::mutex mutex_;
    std::array<Command,kCapacity> ring_{};
    std::array<uint32_t,5> last_sequence_{};
    std::thread::id owner_{};
    bool bound_=false;
    uint32_t scene_=0;
    std::size_t head_=0,count_=0;
public:
    // Only the owning game thread may switch scenes. A switch invalidates all
    // queued old-scene commands and their sequence history.
    bool BindScene(uint32_t epoch) noexcept {
        if(!epoch) return false;
        std::lock_guard<std::mutex> guard(mutex_);
        const auto tid=std::this_thread::get_id();
        if(bound_ && owner_!=tid) return false;
        if(!bound_) {owner_=tid; bound_=true;}
        if(scene_!=epoch) {
            scene_=epoch; head_=0; count_=0; last_sequence_={};
        }
        return true;
    }
    // Transport side may enqueue only validated values for the active scene.
    // This is NOT authentication; source validation must precede this method.
    bool Offer(const Command& c) noexcept {
        if(!Valid(c)) return false;
        std::lock_guard<std::mutex> guard(mutex_);
        if(!bound_ || c.scene!=scene_ || count_==kCapacity) return false;
        ring_[(head_+count_)%kCapacity]=c;
        ++count_;
        return true;
    }
    template<class Fn>
    std::size_t Drain(Fn&& fn) {
        // Only the pinned game thread may retrieve commands; try-lock avoids
        // waiting on a transport worker while executing a game frame.
        std::array<Command,kMaxDrain> batch{};
        std::size_t delivered=0;
        {
            std::unique_lock<std::mutex> guard(mutex_,std::try_to_lock);
            if(!guard.owns_lock() || !bound_ || owner_!=std::this_thread::get_id()) return 0;
            std::size_t examined=0;
            while(count_ && examined<kMaxDrain) {
                const Command c=ring_[head_];
                head_=(head_+1)%kCapacity;
                --count_; ++examined;
                if(c.scene!=scene_ || c.sequence<=last_sequence_[c.player]) continue;
                last_sequence_[c.player]=c.sequence;
                batch[delivered++]=c;
            }
        } // Never call a future game handler while holding the network mutex.
        for(std::size_t i=0;i<delivered;++i) fn(batch[i]);
        return delivered;
    }
    std::size_t Pending() const noexcept {
        std::lock_guard<std::mutex> guard(mutex_);
        return count_;
    }
};
} // namespace bo_bridge
