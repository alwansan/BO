// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// POSIX IPv4 UDP LAN-discovery lab adapter, Android NDK / Linux compatible.
// NOT authenticated, no background server, not connected to BLACK.
#pragma once
#include "lan_discovery.hpp"
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

namespace bo::lan {
struct Source {
    uint32_t ip_network_order=0;
    uint16_t port=0;
    bool operator==(const Source&) const=default;
};
class Socket {
    int fd_=-1;
    uint16_t port_=0;
public:
    Socket()=default;
    Socket(const Socket&)=delete;
    Socket& operator=(const Socket&)=delete;
    ~Socket(){Close();}
    void Close() noexcept {
        if(fd_>=0)::close(fd_);
        fd_=-1;port_=0;
    }
    bool Open(uint16_t port=0,bool loopback_only=true) noexcept {
        if(fd_>=0 || (port!=0 && port<1024))return false;
        int sock=::socket(AF_INET,SOCK_DGRAM,0);
        if(sock<0)return false;
        // Deliberately NO SO_REUSEADDR / SO_REUSEPORT: one UDP port
        // must not be consumed by both gameplay and discovery services.
        sockaddr_in addr{};
        addr.sin_family=AF_INET;
        addr.sin_port=htons(port);
        addr.sin_addr.s_addr=htonl(loopback_only?INADDR_LOOPBACK:INADDR_ANY);
        if(::bind(sock,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))!=0){
            ::close(sock);return false;
        }
        socklen_t len=sizeof(addr);
        if(::getsockname(sock,reinterpret_cast<sockaddr*>(&addr),&len)!=0||
           !ntohs(addr.sin_port)){
            ::close(sock);return false;
        }
        fd_=sock;port_=ntohs(addr.sin_port);
        return true;
    }
    uint16_t Port() const noexcept {return port_;}
    bool SendTo(Source remote,const Bytes& bytes) noexcept {
        if(fd_<0 || !remote.ip_network_order || !remote.port ||
           bytes.empty() || bytes.size()>15+kMaxRoomName) return false;
        sockaddr_in addr{};
        addr.sin_family=AF_INET;
        addr.sin_addr.s_addr=remote.ip_network_order;
        addr.sin_port=htons(remote.port);
        return ::sendto(fd_,bytes.data(),bytes.size(),0,
                        reinterpret_cast<sockaddr*>(&addr),sizeof(addr))
                ==static_cast<ssize_t>(bytes.size());
    }
    bool Receive(Message& out,Source& sender,int timeout_ms=250) noexcept {
        if(fd_<0 || timeout_ms<0 || timeout_ms>2000)return false;
        timeval delay{};
        delay.tv_sec=timeout_ms/1000;
        delay.tv_usec=(timeout_ms%1000)*1000;
        if(::setsockopt(fd_,SOL_SOCKET,SO_RCVTIMEO,&delay,sizeof(delay))!=0)
            return false;
        std::array<uint8_t,64> raw{};
        sockaddr_in addr{};socklen_t len=sizeof(addr);
        // MSG_TRUNC returns datagram length on Linux/Android. Reject rather
        // than treating a partial packet as a valid discovery record.
        const ssize_t got=::recvfrom(fd_,raw.data(),raw.size(),MSG_TRUNC,
                      reinterpret_cast<sockaddr*>(&addr),&len);
        if(got<=0 || got>static_cast<ssize_t>(raw.size()) ||
           len!=sizeof(addr) || addr.sin_family!=AF_INET)
            return false;
        Message decoded{};
        if(!Decode(raw.data(),static_cast<std::size_t>(got),decoded))return false;
        Source src{addr.sin_addr.s_addr,ntohs(addr.sin_port)};
        if(!src.ip_network_order || !src.port) return false;
        out=std::move(decoded);sender=src;
        return true;
    }
};
// Fixed-size inbound rate gate. This is an abuse *mitigation*, not a
// defense against forged LAN packets or spoofed IPs; no identity claimed.
class PeerRateGate {
    struct Entry {Source peer{};uint64_t last=0;bool used=false;};
    std::array<Entry,16> entries_{};
public:
    bool Allow(Source source,uint64_t time_ms) noexcept {
        if(!source.ip_network_order || !source.port ||
           time_ms>UINT64_MAX-1000)return false;
        for(auto& e:entries_){
            if(e.used && e.peer==source){
                if(time_ms<e.last || time_ms-e.last<500)return false;
                e.last=time_ms;return true;
            }
        }
        for(auto& e:entries_){
            if(!e.used || (time_ms>=e.last && time_ms-e.last>=5000)){
                e={source,time_ms,true};return true;
            }
        }
        return false;
    }
};
// Caller must Decode and validate before Respond; never run on BLACK's
// render/game thread and never turn a discovery packet into JOIN authority.
class Responder {
    PeerRateGate inbound_;
    RateGate outgoing_;
public:
    bool Respond(const Message& request,Source peer,uint64_t now_ms,
                 uint16_t game_port,uint16_t discovery_port,
                 uint8_t players,uint8_t capacity,std::string_view room,
                 Bytes& output) noexcept {
        if(request.type!=Type::Query || !request.challenge ||
           !ValidPorts(game_port,discovery_port) ||
           !inbound_.Allow(peer,now_ms) || !outgoing_.Allow(now_ms))
            return false;
        const auto encoded=Reply(request.challenge,game_port,players,capacity,room);
        if(encoded.empty())return false;
        output=encoded;
        return true;
    }
};
} // namespace bo::lan
