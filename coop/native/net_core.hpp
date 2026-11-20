// BO-COOP isolated transport/protocol laboratory, NOT production game netcode.
// No encryption/authentication; do not expose to public internet.
// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace bo {
constexpr std::size_t kMaxPlayers=4; // includes the host
constexpr uint64_t kPeerTimeoutMs=1500;
constexpr uint8_t kVersion=1;

enum class Type : uint8_t { Join=1, Accept=2, Input=3, Snapshot=4, Reject=5 };
struct Actor { uint8_t id=0; int32_t x=0, y=0; };
struct Packet {
    Type type=Type::Join;
    uint64_t nonce=0, token=0;
    uint32_t sequence=0, tick=0;
    uint8_t id=0, count=0, reason=0;
    int8_t dx=0, dy=0;
    std::array<Actor,kMaxPlayers> actors{};
};
using Bytes=std::vector<uint8_t>;

inline void U32(Bytes& b,uint32_t v) {
    for(int i=0;i<4;++i) b.push_back(static_cast<uint8_t>(v>>(8*i)));
}
inline void U64(Bytes& b,uint64_t v) {
    for(int i=0;i<8;++i) b.push_back(static_cast<uint8_t>(v>>(8*i)));
}
inline bool R32(const uint8_t* d,std::size_t n,std::size_t& at,uint32_t& v) {
    if(at>n || n-at<4) return false;
    v=0;
    for(int i=0;i<4;++i) v|=uint32_t(d[at++])<<(8*i);
    return true;
}
inline bool R64(const uint8_t* d,std::size_t n,std::size_t& at,uint64_t& v) {
    if(at>n || n-at<8) return false;
    v=0;
    for(int i=0;i<8;++i) v|=uint64_t(d[at++])<<(8*i);
    return true;
}
inline Bytes Prefix(Type type) {
    return {'B','C','O','P',kVersion,static_cast<uint8_t>(type)};
}
inline Bytes Join(uint64_t nonce) {
    auto b=Prefix(Type::Join); U64(b,nonce); return b;
}
inline Bytes Accept(uint64_t nonce,uint64_t token,uint8_t id) {
    auto b=Prefix(Type::Accept); U64(b,nonce); U64(b,token); b.push_back(id); return b;
}
inline Bytes Input(uint64_t token,uint32_t sequence,int8_t dx,int8_t dy) {
    auto b=Prefix(Type::Input); U64(b,token); U32(b,sequence);
    b.push_back(static_cast<uint8_t>(dx)); b.push_back(static_cast<uint8_t>(dy)); return b;
}
inline Bytes Reject(uint8_t reason) {
    auto b=Prefix(Type::Reject); b.push_back(reason); return b;
}
inline Bytes Snapshot(uint32_t tick,const std::array<Actor,kMaxPlayers>& a,uint8_t count) {
    auto b=Prefix(Type::Snapshot); U32(b,tick); b.push_back(count);
    for(uint8_t i=0;i<count;++i) {
        b.push_back(a[i].id);
        U32(b,std::bit_cast<uint32_t>(a[i].x));
        U32(b,std::bit_cast<uint32_t>(a[i].y));
    }
    return b;
}
inline bool Decode(const uint8_t* d,std::size_t n,Packet& p) {
    if(n<6 || n>512 || d[0]!='B'||d[1]!='C'||d[2]!='O'||d[3]!='P'||d[4]!=kVersion)
        return false;
    Packet v;
    v.type=static_cast<Type>(d[5]);
    std::size_t at=6;
    switch(v.type) {
      case Type::Join:
        if(n!=14 || !R64(d,n,at,v.nonce) || !v.nonce) return false;
        break;
      case Type::Accept:
        if(n!=23 || !R64(d,n,at,v.nonce) || !R64(d,n,at,v.token)) return false;
        v.id=d[at++];
        if(!v.nonce || !v.token || v.id<2 || v.id>4) return false;
        break;
      case Type::Input:
        if(n!=20 || !R64(d,n,at,v.token) || !R32(d,n,at,v.sequence)) return false;
        v.dx=static_cast<int8_t>(d[at++]); v.dy=static_cast<int8_t>(d[at++]);
        if(!v.token || !v.sequence || v.dx< -1 || v.dx>1 || v.dy< -1 || v.dy>1)
            return false;
        break;
      case Type::Snapshot: {
        if(n<11 || !R32(d,n,at,v.tick)) return false;
        v.count=d[at++];
        if(v.count<1 || v.count>kMaxPlayers || n!=11+9*std::size_t(v.count)) return false;
        std::array<bool,kMaxPlayers+1> seen{};
        for(uint8_t i=0;i<v.count;++i) {
            v.actors[i].id=d[at++];
            auto id=v.actors[i].id;
            if(id<1 || id>kMaxPlayers || seen[id]) return false;
            seen[id]=true;
            uint32_t x=0,y=0;
            if(!R32(d,n,at,x) || !R32(d,n,at,y)) return false;
            v.actors[i].x=std::bit_cast<int32_t>(x);
            v.actors[i].y=std::bit_cast<int32_t>(y);
        }
        if(!seen[1]) return false;
        break;
      }
      case Type::Reject:
        if(n!=7 || (v.reason=d[at++])>2) return false;
        break;
      default: return false;
    }
    if(at!=n) return false;
    p=v; return true;
}
inline bool Decode(const Bytes& data,Packet& p) {
    return Decode(data.data(),data.size(),p);
}

struct Outbound { uint64_t endpoint=0; Bytes bytes; };
class Host {
    struct Peer {
        bool used=false;
        uint64_t endpoint=0,nonce=0,token=0,last_seen=0;
        uint32_t seq=0;
        int32_t x=0,y=0;
    };
    std::array<Peer,kMaxPlayers-1> peers_{};
    uint64_t rng_;
    uint32_t tick_=0;
    uint64_t RandomToken() {
        // Test nonce, NOT a cryptographic authenticator.
        rng_^=rng_>>12; rng_^=rng_<<25; rng_^=rng_>>27;
        uint64_t result=rng_*UINT64_C(2685821657736338717);
        return result ? result : 1;
    }
    void Expire(uint64_t now) {
        for(auto& p:peers_)
            if(p.used && now>=p.last_seen && now-p.last_seen>=kPeerTimeoutMs) p={};
    }
public:
    explicit Host(uint64_t seed):rng_(seed ? seed : UINT64_C(0xabcddcbafedcba11)) {}
    std::vector<Outbound> Receive(uint64_t endpoint,const Bytes& bytes,uint64_t now) {
        Expire(now);
        Packet packet;
        if(!endpoint || !Decode(bytes,packet)) return {};
        if(packet.type==Type::Join) {
            for(std::size_t i=0;i<peers_.size();++i) {
                auto& p=peers_[i];
                if(p.used && p.endpoint==endpoint) {
                    if(p.nonce!=packet.nonce) return {{endpoint,Reject(2)}};
                    p.last_seen=now; // idempotent JOIN retry
                    return {{endpoint,Accept(p.nonce,p.token,static_cast<uint8_t>(i+2))}};
                }
            }
            for(std::size_t i=0;i<peers_.size();++i) {
                auto& p=peers_[i];
                if(!p.used) {
                    p.used=true; p.endpoint=endpoint; p.nonce=packet.nonce;
                    p.token=RandomToken(); p.last_seen=now;
                    return {{endpoint,Accept(p.nonce,p.token,static_cast<uint8_t>(i+2))}};
                }
            }
            return {{endpoint,Reject(1)}};
        }
        if(packet.type!=Type::Input) return {};
        for(auto& p:peers_) {
            if(!p.used || p.endpoint!=endpoint || p.token!=packet.token) continue;
            if(packet.sequence<=p.seq) return {}; // duplicate/stale/out-of-order
            // bounded toy grid; not character physics
            if((packet.dx>0 && p.x>=10000)||(packet.dx<0 && p.x<=-10000)||
               (packet.dy>0 && p.y>=10000)||(packet.dy<0 && p.y<=-10000))
                return {};
            p.seq=packet.sequence; p.x+=packet.dx; p.y+=packet.dy; p.last_seen=now;
            std::array<Actor,kMaxPlayers> a{};
            a[0]={1,0,0}; uint8_t count=1;
            for(std::size_t i=0;i<peers_.size();++i) {
                const auto& q=peers_[i];
                if(q.used) a[count++]={static_cast<uint8_t>(i+2),q.x,q.y};
            }
            Bytes snap=Snapshot(++tick_,a,count);
            std::vector<Outbound> out;
            for(const auto& q:peers_) if(q.used) out.push_back({q.endpoint,snap});
            return out;
        }
        return {};
    }
    std::size_t RemoteCount() const {
        std::size_t n=0;
        for(const auto& p:peers_) if(p.used) ++n;
        return n;
    }
};
} // namespace bo
