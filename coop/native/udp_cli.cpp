// BO-COOP isolated UDP lab command-line tool (Android/Linux), NOT a game server.
// No encryption or robust authentication. Only use on trusted LAN/ZeroTier.
// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "net_core.hpp"
#include <arpa/inet.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

using namespace bo;

namespace {
struct Socket {
    int fd=-1;
    Socket() {
        fd=socket(AF_INET,SOCK_DGRAM,0);
        if(fd<0) throw std::runtime_error("socket failed");
    }
    ~Socket(){ if(fd>=0) close(fd); }
    Socket(const Socket&)=delete;
    Socket& operator=(const Socket&)=delete;
};
sockaddr_in Address(const std::string& ip,uint16_t port) {
    sockaddr_in a{};
    a.sin_family=AF_INET; a.sin_port=htons(port);
    if(inet_pton(AF_INET,ip.c_str(),&a.sin_addr)!=1) throw std::runtime_error("bad IPv4 address");
    return a;
}
uint64_t Key(const sockaddr_in& a) {
    return (uint64_t(ntohl(a.sin_addr.s_addr))<<16) | ntohs(a.sin_port);
}
sockaddr_in FromKey(uint64_t key) {
    sockaddr_in a{};
    a.sin_family=AF_INET;
    a.sin_addr.s_addr=htonl(static_cast<uint32_t>(key>>16));
    a.sin_port=htons(static_cast<uint16_t>(key&0xffffu));
    return a;
}
uint64_t NowMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}
uint64_t RandomSeed() {
    std::random_device rd;
    uint64_t x=(uint64_t(rd())<<32)|uint64_t(rd());
    return x ? x : 1;
}
void Transmit(int fd,const sockaddr_in& addr,const Bytes& bytes) {
    if(sendto(fd,bytes.data(),bytes.size(),0,reinterpret_cast<const sockaddr*>(&addr),sizeof(addr))
       !=static_cast<ssize_t>(bytes.size())) throw std::runtime_error("UDP sendto failed");
}
uint16_t Port(const char* value) {
    char* end=nullptr;
    auto n=std::strtoul(value,&end,10);
    if(!value[0] || !end || *end || n<1 || n>65535) throw std::runtime_error("bad UDP port");
    return static_cast<uint16_t>(n);
}
void HostLoop(const std::string& ip,uint16_t port) {
    Socket socket;
    auto addr=Address(ip,port);
    if(bind(socket.fd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))<0)
        throw std::runtime_error("bind failed: port already used or unavailable");
    Host engine(RandomSeed());
    std::cout<<"BO-COOP LAB host "<<ip<<":"<<port<<"\n"
             <<"NO ENCRYPTION: trusted private network only; NEVER forward this port publicly\n"
             <<"Press Ctrl+C to stop. No game data is transmitted.\n"<<std::flush;
    while(true) {
        uint8_t buffer[512];
        sockaddr_in from{}; socklen_t from_len=sizeof(from);
        ssize_t n=recvfrom(socket.fd,buffer,sizeof(buffer),0,
                           reinterpret_cast<sockaddr*>(&from),&from_len);
        if(n<0) continue; // interrupted receive, no game loop to block
        Bytes message(buffer,buffer+n);
        const auto result=engine.Receive(Key(from),message,NowMs());
        for(const auto& reply:result) Transmit(socket.fd,FromKey(reply.endpoint),reply.bytes);
    }
}
Packet Receive(int fd) {
    uint8_t buffer[512];
    const auto n=recv(fd,buffer,sizeof(buffer),0);
    Packet packet;
    if(n<=0 || !Decode(buffer,static_cast<std::size_t>(n),packet))
        throw std::runtime_error("host timeout or invalid reply");
    return packet;
}
void JoinLoop(const std::string& ip,uint16_t port) {
    Socket socket;
    auto address=Address(ip,port);
    if(connect(socket.fd,reinterpret_cast<sockaddr*>(&address),sizeof(address))<0)
        throw std::runtime_error("connect failed");
    timeval timeout{}; timeout.tv_sec=2;
    if(setsockopt(socket.fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout))<0)
        throw std::runtime_error("setsockopt failed");
    auto nonce=RandomSeed();
    Packet accepted;
    bool joined=false;
    for(int attempt=0;attempt<3 && !joined;++attempt) {
        Transmit(socket.fd,address,Join(nonce));
        try {
            auto reply=Receive(socket.fd);
            if(reply.type==Type::Reject) throw std::runtime_error("room full or endpoint busy");
            if(reply.type==Type::Accept && reply.nonce==nonce) {
                accepted=reply; joined=true;
            }
        } catch(const std::runtime_error& e) {
            if(std::string(e.what())!="host timeout or invalid reply") throw;
        }
    }
    if(!joined) throw std::runtime_error("no host response after 3 attempts");
    std::cout<<"Connected as simulated player "<<unsigned(accepted.id)
             <<" to "<<ip<<":"<<port<<"\n";
    for(uint32_t seq=1;seq<=10;++seq) {
        Transmit(socket.fd,address,Input(accepted.token,seq,1,0));
        try {
            const auto snap=Receive(socket.fd);
            if(snap.type!=Type::Snapshot) throw std::runtime_error("not snapshot");
            std::cout<<"snapshot tick="<<snap.tick<<" simulated actors="<<unsigned(snap.count)<<"\n";
            for(uint8_t i=0;i<snap.count;++i)
                std::cout<<"  player "<<unsigned(snap.actors[i].id)
                         <<" x="<<snap.actors[i].x<<" y="<<snap.actors[i].y<<"\n";
        } catch(const std::runtime_error& e) {
            std::cerr<<"Snapshot missing/invalid: "<<e.what()<<"\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
}
int main(int argc,char** argv) {
    try {
        if(argc==4 && std::string(argv[1])=="host")
            HostLoop(argv[2],Port(argv[3]));
        else if(argc==4 && std::string(argv[1])=="join")
            JoinLoop(argv[2],Port(argv[3]));
        else {
            std::cerr<<"BO-COOP LAB ONLY (no BLACK integration)\n"
                     <<"  "<<argv[0]<<" host 127.0.0.1 24872\n"
                     <<"  "<<argv[0]<<" host 0.0.0.0 24872   # TRUSTED LAN/ZEROTIER ONLY\n"
                     <<"  "<<argv[0]<<" join HOST_IPV4 24872\n";
            return 2;
        }
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"ERROR: "<<e.what()<<"\n";
        return 1;
    }
}
