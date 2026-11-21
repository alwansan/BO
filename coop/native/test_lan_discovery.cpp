#include "lan_discovery.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
using namespace bo::lan;
void TestWire() {
    Message m{};
    auto q=Query(0x332211ee);
    assert(q.size()==10 && Decode(q,m));
    assert(m.type==Type::Query && m.challenge==0x332211ee);
    assert(Query(0).empty());
    for(std::size_t i=0;i<q.size();++i)assert(!Decode(q.data(),i,m));
    auto hello=Reply(0x332211ee,1508,1,4,"BLACK Co-op");
    assert(!hello.empty() && Decode(hello,m));
    assert(m.type==Type::Reply && m.game_port==1508 &&
           m.capacity==4 && m.occupied==1 && m.name=="BLACK Co-op");
    assert(IsReplyFor(m,0x332211ee,1510));
    assert(!IsReplyFor(m,0x33221100,1510));
    assert(!IsReplyFor(m,0x332211ee,1508));
    hello.push_back(0);assert(!Decode(hello,m));
    hello.pop_back();
    hello[14]=33;assert(!Decode(hello,m));
    hello[14]=11;
    hello[15]='\n';assert(!Decode(hello,m));
    hello[15]='B';
    hello[4]=2;assert(!Decode(hello,m));
    hello[4]=kVersion;
    hello[5]=99;assert(!Decode(hello,m));
    assert(!Decode(nullptr,10,m));
    assert(!Decode(Bytes{},m));
    std::cout<<"PASS LAN query/reply bytes, strict framing, challenge and version\n";
}
void TestGuards() {
    assert(ValidPorts(1508,1510));
    assert(!ValidPorts(1508,1508));
    assert(!ValidPorts(0,1510));
    assert(!ValidPorts(80,1510));
    assert(!ValidPorts(1508,70000));
    assert(!ValidPorts(1508,0));
    assert(Reply(0,1508,1,4,"ROOM").empty());
    assert(Reply(1,80,1,4,"ROOM").empty());
    assert(Reply(1,1508,0,4,"ROOM").empty());
    assert(Reply(1,1508,4,3,"ROOM").empty());
    assert(Reply(1,1508,1,5,"ROOM").empty());
    assert(Reply(1,1508,1,4,"").empty());
    assert(Reply(1,1508,1,4,"a\nb").empty());
    assert(Reply(1,1508,1,4,"a:b").empty());
    assert(Reply(1,1508,1,4,std::string(33,'a')).empty());
    RateGate gate;
    assert(gate.Allow(0));
    assert(!gate.Allow(0));
    assert(!gate.Allow(999));
    assert(gate.Allow(1000));
    assert(!gate.Allow(1000));
    assert(gate.Allow(2000));
    assert(!gate.Allow(1999));
    gate.Reset();
    assert(gate.Allow(1));
    std::cout<<"PASS distinct ports, bounded room names and outgoing rate gate\n";
}
void FuzzDecoder() {
    std::mt19937 gen(0xabcddcba);
    Message x{};
    for(unsigned i=0;i<5000;++i) {
        const auto len=gen()%75;
        Bytes data(len);
        for(auto& b:data)b=static_cast<uint8_t>(gen());
        const bool valid=Decode(data,x);
        if(valid){
            assert(x.challenge!=0);
            assert(x.type==Type::Query || x.type==Type::Reply);
            assert(x.type==Type::Query || ValidRoom(x.name));
        }
    }
    std::cout<<"PASS 5000 deterministic malformed packet probes\n";
}
int main(){TestWire();TestGuards();FuzzDecoder();
    std::cout<<"NET-LAN discovery codec LAB PASS; no sockets, browser or gameplay\n";
}
