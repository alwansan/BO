#include "mission_barrier.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using namespace bo_story;
int main(){
    HostMissionBarrier host;GuestMissionBarrier guest,third;
    const MissionTicket a{31,751,7,1,900,0b0111};
    MissionTicket commit{};
    assert(!host.Prepare(a,false)); // guest cannot select mission
    assert(host.Prepare(a,true));
    assert(!host.Commit(commit)); // roster 2/4 or 3/4 alone is not START
    assert(!host.Ready(a,2,900,false)); // untrusted peer
    assert(!host.Ready(a,2,899,true)); // loaded wrong scene
    assert(!host.Ready(a,4,900,true)); // not a roster member
    assert(!guest.AcceptPrepare(a,2,false));
    assert(guest.AcceptPrepare(a,2,true));
    assert(third.AcceptPrepare(a,3,true));
    assert(!guest.MarkActuallyLoaded(8,1,900));
    assert(!guest.MarkActuallyLoaded(7,2,900));
    assert(!guest.MarkActuallyLoaded(7,1,899));
    assert(!guest.AcceptCommit(a,true)); // commit before READY prohibited
    assert(guest.MarkActuallyLoaded(7,1,900));
    assert(third.MarkActuallyLoaded(7,1,900));
    assert(!guest.AcceptCommit(a,false)); // forged host
    assert(host.Ready(a,1,900,true));
    assert(host.Ready(a,2,900,true));
    assert(!host.Ready(a,2,900,true)); // duplicate
    assert(!host.Commit(commit)); // still waiting on third phone
    assert(host.Ready(a,3,900,true));
    assert(host.ReadyMask()==a.participants);
    assert(host.Commit(commit)&&commit==a);
    assert(!host.Commit(commit)); // one-shot
    assert(guest.AcceptCommit(commit,true));
    assert(third.AcceptCommit(commit,true));
    assert(!guest.AcceptCommit(commit,true));
    assert(guest.Committed()&&third.Committed()&&host.Committed());
    bool foreign=false;
    std::thread worker([&]{foreign=host.Reset();});
    worker.join();assert(!foreign);
    assert(host.Reset()&&guest.Reset()&&third.Reset());
    // A stale READY from an earlier mission cannot authorize a later mission.
    const MissionTicket b{31,752,7,2,901,0b0011};
    assert(host.Prepare(b,true));
    assert(!host.Ready(a,2,900,true));
    assert(host.Ready(b,1,901,true));
    assert(!host.Commit(commit));
    assert(host.RemovePeer(2)); // do not silently switch to single-player
    assert(!host.Commit(commit));
    assert(!host.Prepare(MissionTicket{31,0,7,1,903,3},true));
    assert(!host.Prepare(MissionTicket{31,753,7,1,903,1},true));
    assert(host.Prepare(MissionTicket{31,753,7,1,903,3},true));
    assert(host.Reset());
    // The FIRST campaign mission is index 0, and stage 0 is legitimate.
    // This exact bug previously rejected the first selectable mission.
    const MissionTicket first{44,333,0,0,1,0b0011};
    assert(host.Prepare(first,true));
    assert(guest.AcceptPrepare(first,2,true));
    assert(guest.MarkActuallyLoaded(0,0,1));
    assert(host.Ready(first,1,1,true));
    assert(host.Ready(first,2,1,true));
    assert(host.Commit(commit)&&commit==first);
    assert(guest.AcceptCommit(commit,true));
    assert(host.Reset()&&guest.Reset());
    assert(!host.Prepare(MissionTicket{45,777,8,0,1,3},true)); // no mission #8
    std::cout<<"MISSION_BARRIER_PASS: host ticket, all connected peers loaded exact scene,"
                " authenticated ready/commit, cancellation and replay fences\n";
    std::cout<<"No BLACK engine Start, game scene loader, or live TLS wiring claimed\n";
}
