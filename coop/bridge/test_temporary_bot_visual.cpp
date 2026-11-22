#include "temporary_bot_visual.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using namespace bo_bridge;
static bo::Packet Snap(uint32_t tick,std::initializer_list<bo::Actor> actors){
    std::array<bo::Actor,bo::kMaxPlayers> a{};
    uint8_t n=0;for(const auto& actor:actors){assert(n<4);a[n++]=actor;}
    bo::Packet packet{};assert(bo::Decode(bo::Snapshot(tick,a,n),packet));
    return packet;
}
int main(){
    RemoteAllyRoster host,guest;
    TemporaryBotVisualPolicy forHost,forGuest;
    TemporaryBotVisual model{};
    assert(!forHost.BindVerifiedScene(9,1,false));
    assert(forHost.BindVerifiedScene(9,1,true));
    assert(forGuest.BindVerifiedScene(9,2,true));
    assert(host.BindScene(9,1)&&guest.BindScene(9,2));
    assert(host.ApplyTrustedSnapshot(Snap(1,{{1,0,0},{2,3,5},{3,4,6}}),9,100));
    assert(guest.ApplyTrustedSnapshot(Snap(1,{{1,0,0},{2,3,5},{3,4,6}}),9,100));
    AvatarView two{},three{},one{};
    assert(host.ReadRemote(2,100,two)&&host.ReadRemote(3,100,three));
    assert(guest.ReadRemote(1,100,one));
    // "Enemy" and "ally" labels no longer matter: use any validated bot
    // LOOK without installing the bot brain into a human player.
    const uint64_t arbitraryEnemyModel=0xA155u;
    assert(!forHost.Select(host,2,two.generation,100,arbitraryEnemyModel,false,true,model));
    assert(!forHost.Select(host,2,two.generation,100,0,true,true,model));
    assert(!forHost.Select(host,2,two.generation,100,arbitraryEnemyModel,true,false,model));
    assert(!forHost.Select(host,1,two.generation,100,arbitraryEnemyModel,true,true,model));
    assert(!forHost.Select(host,2,two.generation+1,100,arbitraryEnemyModel,true,true,model));
    assert(forHost.Select(host,2,two.generation,100,arbitraryEnemyModel,true,true,model));
    assert(model.remoteHuman==2&&model.temporaryLook&&model.independentlyControlledHuman);
    assert(!model.ownsAiBrain&&model.originalBotModel==arbitraryEnemyModel);
    const uint64_t arbitraryFriendlyBotModel=0xB966u;
    assert(forHost.Select(host,3,three.generation,100,arbitraryFriendlyBotModel,true,true,model));
    assert(model.remoteHuman==3&&model.originalBotModel==arbitraryFriendlyBotModel);
    assert(forGuest.Select(guest,1,one.generation,100,arbitraryEnemyModel,true,true,model));
    assert(model.remoteHuman==1&&model.originalBotModel==arbitraryEnemyModel);
    assert(!forGuest.Select(guest,2,one.generation,100,arbitraryEnemyModel,true,true,model));
    assert(!forHost.Select(host,2,two.generation,101+bo::kPeerTimeoutMs,
                           arbitraryEnemyModel,true,true,model));
    // Same slot after leave/rejoin must select only the NEW live generation.
    assert(host.ApplyTrustedSnapshot(Snap(2,{{1,0,0},{3,3,3}}),9,200));
    assert(!forHost.Select(host,2,two.generation,200,arbitraryEnemyModel,true,true,model));
    assert(host.ApplyTrustedSnapshot(Snap(3,{{1,0,0},{2,7,9}}),9,300));
    assert(host.ReadRemote(2,300,two));
    assert(!forHost.Select(host,2,1,300,arbitraryEnemyModel,true,true,model));
    assert(forHost.Select(host,2,two.generation,300,arbitraryEnemyModel,true,true,model));
    // Stage transition drops every old remote placeholder.
    assert(forHost.BindVerifiedScene(10,1,true));
    assert(!forHost.Select(host,2,two.generation,300,arbitraryEnemyModel,true,true,model));
    bool foreign=true;
    std::thread worker([&]{foreign=forHost.Select(host,2,two.generation,300,
                                                   arbitraryEnemyModel,true,true,model);});
    worker.join();assert(!foreign);
    std::cout<<"TEMP_BOT_LOOK_PASS: ANY verified bot visual candidate for peer2/3 and guest-visible host, never NPC brain, stale scene/rejoin gated\n";
    std::cout<<"NO game actor, original mesh rendering, original story stage or real gameplay integration\n";
}
