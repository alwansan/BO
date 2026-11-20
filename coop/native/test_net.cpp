#include "net_core.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
using namespace bo;

Packet Read(const Bytes& bytes) {
    Packet p; assert(Decode(bytes,p)); return p;
}
void TestWire() {
    Packet p;
    auto a=Join(42);
    assert(Decode(a,p) && p.nonce==42 && p.type==Type::Join);
    for(std::size_t n=0;n<a.size();++n) assert(!Decode(a.data(),n,p));
    a[0]=0; assert(!Decode(a,p));
    a=Input(8,1,2,0); assert(!Decode(a,p));
    a=Input(8,1,-1,1); assert(Decode(a,p) && p.dx==-1 && p.dy==1);
    a=Accept(12,13,4); assert(Decode(a,p) && p.id==4 && p.token==13);
    std::array<Actor,kMaxPlayers> arr{{{1,0,0},{2,-110,34},{3,11,-7},{4,0,0}}};
    a=Snapshot(11,arr,4); assert(Decode(a,p) && p.tick==11 && p.count==4 &&
                                   p.actors[1].x==-110 && p.actors[2].y==-7);
    a.push_back(0); assert(!Decode(a,p));
    a=Snapshot(11,arr,4); a[20]=1; // duplicate the host actor ID
    assert(!Decode(a,p));
    a=Join(0); assert(!Decode(a,p));
    std::cout<<"PASS strict packet codec\n";
}
void TestState() {
    Host h(123);
    auto j1=h.Receive(100,Join(101),0), j2=h.Receive(101,Join(102),0);
    assert(j1.size()==1 && j2.size()==1);
    auto p1=Read(j1[0].bytes),p2=Read(j2[0].bytes);
    assert(p1.id==2 && p2.id==3 && p1.token!=p2.token);
    assert(Read(h.Receive(100,Join(101),500)[0].bytes).token==p1.token);
    assert(Read(h.Receive(100,Join(999),500)[0].bytes).reason==2);
    assert(Read(h.Receive(102,Join(103),500)[0].bytes).id==4);
    assert(Read(h.Receive(103,Join(104),500)[0].bytes).reason==1);
    assert(h.RemoteCount()==3);
    assert(h.Receive(999,Input(p1.token,1,1,0),700).empty());
    assert(h.Receive(100,Input(p2.token,1,1,0),700).empty());
    auto updates=h.Receive(100,Input(p1.token,1,1,0),700);
    assert(updates.size()==3 && Read(updates[0].bytes).count==4);
    auto s=Read(updates[0].bytes); assert(s.actors[1].id==2 && s.actors[1].x==1);
    assert(h.Receive(100,Input(p1.token,1,1,0),750).empty());
    assert(h.Receive(100,Input(p1.token,0,1,0),750).empty());
    assert(h.Receive(100,Input(p1.token,2,8,0),750).empty());
    assert(h.Receive(100,Input(p1.token,3,-1,0),750).size()==3);
    assert(h.Receive(100,Input(p1.token,2,1,0),800).empty());
    // expiry: 101/102 idle, 100 active
    auto again=h.Receive(100,Input(p1.token,4,1,0),2101);
    assert(again.size()==1 && Read(again[0].bytes).count==2);
    assert(h.RemoteCount()==1);
    auto newjoin=h.Receive(101,Join(222),2102);
    assert(Read(newjoin[0].bytes).id==3);
    assert(h.Receive(101,Input(p2.token,5,1,0),2103).empty());
    assert(h.Receive(101,Input(Read(newjoin[0].bytes).token,1,1,0),2103).size()==2);
    std::cout<<"PASS 3 peers, capacity, dedup, ordering, token, timeout/rejoin\n";
}
struct Socket {
    int fd=-1;
    Socket() { fd=socket(AF_INET,SOCK_DGRAM,0); assert(fd>=0); }
    ~Socket(){if(fd>=0) close(fd);}
    Socket(const Socket&)=delete;
    Socket& operator=(const Socket&)=delete;
};
uint64_t Key(sockaddr_in a) {
    return (uint64_t(ntohl(a.sin_addr.s_addr))<<16) | ntohs(a.sin_port);
}
sockaddr_in Addr(uint16_t port) {
    sockaddr_in a{};
    a.sin_family=AF_INET; a.sin_addr.s_addr=htonl(INADDR_LOOPBACK); a.sin_port=htons(port);
    return a;
}
uint16_t BoundPort(Socket& s) {
    auto a=Addr(0); assert(bind(s.fd,reinterpret_cast<sockaddr*>(&a),sizeof(a))==0);
    socklen_t n=sizeof(a); assert(getsockname(s.fd,reinterpret_cast<sockaddr*>(&a),&n)==0);
    return ntohs(a.sin_port);
}
void Send(Socket& s,sockaddr_in to,const Bytes& b) {
    assert(sendto(s.fd,b.data(),b.size(),0,reinterpret_cast<sockaddr*>(&to),sizeof(to))==static_cast<ssize_t>(b.size()));
}
Packet Recv(Socket& s) {
    uint8_t b[512]; ssize_t n=recv(s.fd,b,sizeof(b),0);
    assert(n>0); Packet p; assert(Decode(b,static_cast<std::size_t>(n),p)); return p;
}
void ServeOnce(Socket& server,Host& host,uint64_t now) {
    uint8_t buffer[512]; sockaddr_in sender{}; socklen_t len=sizeof(sender);
    ssize_t n=recvfrom(server.fd,buffer,sizeof(buffer),0,reinterpret_cast<sockaddr*>(&sender),&len);
    assert(n>0);
    Bytes incoming(buffer,buffer+n);
    auto result=host.Receive(Key(sender),incoming,now);
    for(const auto& msg:result) {
        sockaddr_in target{};
        target.sin_family=AF_INET;
        target.sin_addr.s_addr=htonl(static_cast<uint32_t>(msg.endpoint>>16));
        target.sin_port=htons(static_cast<uint16_t>(msg.endpoint&65535));
        assert(sendto(server.fd,msg.bytes.data(),msg.bytes.size(),0,
                      reinterpret_cast<sockaddr*>(&target),sizeof(target))==static_cast<ssize_t>(msg.bytes.size()));
    }
}
void TestUdp() {
    Socket server,a,b; auto port=BoundPort(server); BoundPort(a); BoundPort(b);
    timeval timeout{}; timeout.tv_sec=2;
    for(auto* s:{&server,&a,&b}) assert(setsockopt(s->fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout))==0);
    auto address=Addr(port); Host host(91);
    Send(a,address,Join(101)); ServeOnce(server,host,0); auto aj=Recv(a);
    Send(b,address,Join(102)); ServeOnce(server,host,1); auto bj=Recv(b);
    assert(aj.id==2 && bj.id==3);
    Send(a,address,Input(aj.token,1,1,0)); ServeOnce(server,host,2);
    auto sa=Recv(a), sb=Recv(b);
    assert(sa.type==Type::Snapshot && sb.type==Type::Snapshot && sa.tick==sb.tick);
    assert(sa.count==3 && sa.actors[1].x==1 && sb.actors[1].x==1);
    std::cout<<"PASS real UDP IPv4 loopback, host and two clients\n";
}
int main(){
    TestWire(); TestState(); TestUdp();
    std::cout<<"NET-003 C++ LAB PASS (no BLACK integration)\n";
}
