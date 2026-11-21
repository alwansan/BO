// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Cross-platform, self-contained LAN discovery packet codec. NOT sockets.
// Independently authored after studying a Windows TDM design pattern; no
// third-party code/art was copied. NEVER treat discovery as authentication.
#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace bo::lan {
constexpr uint16_t kSuggestedGamePort=1508;
constexpr uint16_t kSuggestedDiscoveryPort=1510;
constexpr uint8_t kVersion=1;
constexpr uint8_t kMaxSlots=4; // Includes phone authoritative host.
constexpr std::size_t kMaxRoomName=32;
enum class Type:uint8_t { Query=1,Reply=2 };
using Bytes=std::vector<uint8_t>;
struct Message {
    Type type=Type::Query;
    uint32_t challenge=0; // Request/response correlation, NOT an auth token.
    uint16_t game_port=0;
    uint8_t occupied=0, capacity=0;
    std::string name;
};
inline bool ValidPorts(uint32_t game_port,uint32_t discovery_port) noexcept {
    // Unprivileged ports, and never let the listener race a game socket.
    return game_port>=1024 && discovery_port>=1024 &&
           game_port<=65535 && discovery_port<=65535 &&
           game_port!=discovery_port;
}
inline bool ValidRoom(std::string_view name) noexcept {
    if(name.empty() || name.size()>kMaxRoomName) return false;
    for(unsigned char c:name)
        if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||
             (c>='0'&&c<='9')||c=='_'||c=='-'||c==' ')) return false;
    return true;
}
inline void U16(Bytes& bytes,uint16_t x) {
    bytes.push_back(static_cast<uint8_t>(x));
    bytes.push_back(static_cast<uint8_t>(x>>8));
}
inline void U32(Bytes& bytes,uint32_t x) {
    for(int i=0;i<4;++i) bytes.push_back(static_cast<uint8_t>(x>>(8*i)));
}
inline uint32_t R32(const uint8_t* b) noexcept {
    return uint32_t(b[0])|(uint32_t(b[1])<<8)|
           (uint32_t(b[2])<<16)|(uint32_t(b[3])<<24);
}
inline Bytes Prefix(Type t,uint32_t challenge) {
    Bytes data{'B','C','L','N',kVersion,static_cast<uint8_t>(t)};
    U32(data,challenge);
    return data;
}
inline Bytes Query(uint32_t challenge) {
    return challenge?Prefix(Type::Query,challenge):Bytes{};
}
inline Bytes Reply(uint32_t challenge,uint16_t port,uint8_t occupied,
                   uint8_t capacity,std::string_view room) {
    if(!challenge || port<1024 || !capacity || capacity>kMaxSlots ||
       !occupied || occupied>capacity || !ValidRoom(room))
        return {};
    Bytes data=Prefix(Type::Reply,challenge);
    U16(data,port);
    data.push_back(occupied);
    data.push_back(capacity);
    data.push_back(static_cast<uint8_t>(room.size()));
    data.insert(data.end(),room.begin(),room.end());
    return data;
}
inline bool Decode(const uint8_t* input,std::size_t size,Message& result) {
    if(!input || size<10 || size>15+kMaxRoomName ||
       input[0]!='B'||input[1]!='C'||input[2]!='L'||input[3]!='N'||
       input[4]!=kVersion) return false;
    Message tmp{};
    tmp.type=static_cast<Type>(input[5]);
    tmp.challenge=R32(input+6);
    if(!tmp.challenge) return false;
    if(tmp.type==Type::Query) {
        if(size!=10)return false;
    } else if(tmp.type==Type::Reply) {
        if(size<16) return false;
        tmp.game_port=uint16_t(input[10])|(uint16_t(input[11])<<8);
        tmp.occupied=input[12];tmp.capacity=input[13];
        const auto count=input[14];
        if(tmp.game_port<1024 || !tmp.capacity || tmp.capacity>kMaxSlots||
           !tmp.occupied || tmp.occupied>tmp.capacity ||
           !count || count>kMaxRoomName || size!=15u+count)
            return false;
        tmp.name.assign(reinterpret_cast<const char*>(input+15),count);
        if(!ValidRoom(tmp.name)) return false;
    } else return false;
    result=std::move(tmp);
    return true;
}
inline bool Decode(const Bytes& input,Message& result) {
    return Decode(input.data(),input.size(),result);
}
// Bound rate of *outgoing* query/reply in the host/GUI event loop. The
// caller needs its own per-source inbound rate cap, OS socket permissions,
// IPv4 LAN scoping, and authenticated session negotiation on JOIN.
class RateGate {
    uint64_t next_allowed_=0;
public:
    bool Allow(uint64_t monotonic_ms) noexcept {
        if(monotonic_ms<next_allowed_) return false;
        if(monotonic_ms>UINT64_MAX-1000) return false;
        next_allowed_=monotonic_ms+1000;
        return true;
    }
    void Reset() noexcept {next_allowed_=0;}
};
inline bool IsReplyFor(const Message& packet,uint32_t expected_challenge,
                       uint16_t discovery_port) noexcept {
    return packet.type==Type::Reply && expected_challenge!=0 &&
           packet.challenge==expected_challenge &&
           ValidPorts(packet.game_port,discovery_port);
}
} // namespace bo::lan
