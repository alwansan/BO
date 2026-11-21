// BO-COOP synthetic composition test. NO actual BLACK, network sockets,
// authentication, actor models or memory writes are involved in this test.
// Network UDP socket tests exist separately under coop/native/test_net.cpp.
#include "scene_inbox.hpp"
#include "remote_proxy_roster.hpp"
#include "../native/net_core.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

struct FakeScene {
    int x2=0,y2=0,x3=0,y3=0;
    void Apply(const bo_bridge::Command& cmd) {
        assert(cmd.scene==41 || cmd.scene==42);
        if(cmd.player==2) {x2+=cmd.move_x/1000; y2+=cmd.move_y/1000;}
        if(cmd.player==3) {x3+=cmd.move_x/1000; y3+=cmd.move_y/1000;}
    }
};
bo::Packet Parsed(const bo::Bytes& bytes) {
    bo::Packet p;
    assert(bo::Decode(bytes,p));
    return p;
}
int main() {
    bo_bridge::SceneInbox inbox;
    FakeScene fake;
    assert(inbox.BindScene(41));
    std::size_t accepted=0;
    std::vector<bo::Packet> staged_snapshots;
    std::thread transport([&] {
        // The host owns the toy authoritative positions and validates endpoints.
        bo::Host host(0x9911);
        auto join2=host.Receive(200,bo::Join(11),0);
        auto join3=host.Receive(300,bo::Join(12),0);
        assert(join2.size()==1 && join3.size()==1);
        const bo::Packet a=Parsed(join2[0].bytes),b=Parsed(join3[0].bytes);
        assert(a.type==bo::Type::Accept && b.type==bo::Type::Accept);
        assert(a.id==2 && b.id==3);

        // Non-owner packets must not enter the mock game queue.
        assert(host.Receive(201,bo::Input(a.token,1,1,0),1).empty());
        assert(host.Receive(200,bo::Input(b.token,1,1,0),1).empty());
        auto deliver=[&](uint64_t endpoint,uint8_t id,uint64_t token,
                         uint32_t seq,int8_t x,int8_t y) {
            const auto wire=bo::Input(token,seq,x,y);
            const auto p=Parsed(wire);
            assert(p.type==bo::Type::Input);
            const auto snapshots=host.Receive(endpoint,wire,10+seq);
            if(snapshots.empty()) return false;
            assert(snapshots.size()==2);
            const auto snap=Parsed(snapshots[0].bytes);
            assert(snap.type==bo::Type::Snapshot && snap.count==3);
            staged_snapshots.push_back(snap); // value-only transport staging
            const bo_bridge::Command c={
                bo_bridge::Kind::RemoteInput,id,41,p.sequence,
                static_cast<int16_t>(p.dx*1000),
                static_cast<int16_t>(p.dy*1000)
            };
            assert(inbox.Offer(c));
            ++accepted;
            return true;
        };
        assert(deliver(200,a.id,a.token,1,1,0));
        assert(deliver(300,b.id,b.token,1,-1,1));
        assert(!deliver(200,a.id,a.token,1,1,0)); // replay rejected at host
        assert(deliver(200,a.id,a.token,2,1,0));
        assert(host.RemoteCount()==2);
        // This worker may not drain on behalf of the game thread.
        assert(inbox.Drain([](const bo_bridge::Command&){assert(false);})==0);
    });
    transport.join();

    // Snapshot state can be applied only by the owner/main test thread.
    // This lab treats the toy host as trusted by construction; that is
    // NOT production session authentication or player2 game integration.
    bo_bridge::RemoteProxyRoster proxies;
    assert(proxies.BindScene(41));
    uint64_t now=100;
    for(const auto& packet: staged_snapshots)
        assert(proxies.ApplyTrustedSnapshot(packet,41,now++));
    bo_bridge::ProxyView proxy{};
    assert(proxies.Read(2,103,proxy) && proxy.x==2 && proxy.y==0);
    assert(proxies.Read(3,103,proxy) && proxy.x==-1 && proxy.y==1);
    assert(!proxies.Read(4,103,proxy));
    assert(!proxies.ApplyTrustedSnapshot(staged_snapshots.front(),41,104));
    // No fake gameplay mutation occurred on the transport thread.
    assert(accepted==3 && inbox.Pending()==3);
    assert(fake.x2==0 && fake.x3==0);
    std::vector<uint8_t> arrival;
    assert(inbox.Drain([&](const bo_bridge::Command& c) {
        arrival.push_back(c.player);
        fake.Apply(c); // simulated actor update, NOT a BLACK actor write.
    })==3);
    assert((arrival==std::vector<uint8_t>{2,3,2}));
    assert(fake.x2==2 && fake.y2==0 && fake.x3==-1 && fake.y3==1);
    assert(inbox.Pending()==0);

    // Switching a mission epoch drops old-scene transport commands atomically.
    assert(inbox.Offer({bo_bridge::Kind::RemoteInput,2,41,3,1000,0}));
    assert(inbox.BindScene(42));
    assert(proxies.BindScene(42));
    assert(!proxies.Read(2,104,proxy));
    assert(inbox.Pending()==0);
    assert(!inbox.Offer({bo_bridge::Kind::RemoteInput,2,41,4,1000,0}));
    assert(inbox.Offer({bo_bridge::Kind::RemoteInput,2,42,1,-1000,0}));
    assert(inbox.Drain([&](const bo_bridge::Command& c) {fake.Apply(c);})==1);
    assert(fake.x2==1 && fake.x3==-1);
    std::cout<<"PASS toy UDP host -> proxy roster + thread inbox -> mock scene\n";
    std::cout<<"No actual networked BLACK gameplay or authenticated transport\n";
}
