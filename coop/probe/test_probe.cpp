#include "probe_core.hpp"
#include <cassert>
#include <algorithm>
#include <cfenv>
#include <cerrno>
#include <cstring>
#include <limits>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

int main() {
    using namespace bo_probe;
    errno=123;
    std::feraiseexcept(FE_INVALID);
    const int flags=std::fetestexcept(FE_ALL_EXCEPT);
    { HostState saved; errno=0; std::feclearexcept(FE_ALL_EXCEPT); }
    assert(errno==123 && std::fetestexcept(FE_ALL_EXCEPT)==flags);
    std::feclearexcept(FE_ALL_EXCEPT);
    const auto page=static_cast<size_t>(sysconf(_SC_PAGESIZE));
    auto* mem=static_cast<unsigned char*>(mmap(nullptr,page*2,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0));
    assert(mem!=MAP_FAILED);
    uint32_t magic=0x12345678;
    std::memcpy(mem,&magic,4);
    GuestReader reader(reinterpret_cast<uintptr_t>(mem)-0x10000);
    uint32_t value=0;
    assert(reader.Read(0x10000,value) && value==0x12345678);
    assert(mprotect(mem+page,page,PROT_NONE)==0);
    uint64_t kept=0x9988776655443322ULL;
    assert(!reader.Read(static_cast<uint32_t>(0x10000+page-4),kept));
    assert(kept==0x9988776655443322ULL); // partial read never leaks into output
    assert(!reader.Read(0xfffffffcu,kept));
    assert(!reader.Read(0,kept));
    assert(!reader.Read(0x10000u + static_cast<uint32_t>(page),kept));
    GuestReader overflow(std::numeric_limits<uintptr_t>::max()-10);
    assert(!overflow.Read(0x10000,kept));
    assert(!reader.Field(0xfffffff0u,0x100u,kept));
    munmap(mem,page*2);

    Registry ids;
    ids.World(100);
    assert(ids.epoch==1);
    for(uint32_t i=0;i<63;++i) assert(ids.Id(1000+i,2000)==i+2);
    assert(ids.Id(1000,2000)==2);
    assert(ids.Id(9000,2000)==0);
    ids.World(100); assert(ids.Id(1000,2000)==2);
    ids.World(0); ids.World(200);
    assert(ids.epoch==3 && ids.Id(9000,2000)==2);
    // A flood of LOD/animation objects must not starve the player hook.
    Registry prioritised;
    prioritised.World(700);
    for(uint32_t i=0;i<63;++i)
        assert(prioritised.Id(3000+i,4000)==i+2);
    assert(prioritised.Id(5000,4000)==0); // all nonlocal slots occupied
    assert(prioritised.Id(6000,5000,true)==1);
    assert(prioritised.Id(6000,5000)==1);
    assert(prioritised.Id(6000,5000,true)==1);
    assert(prioritised.Id(6001,5000,true)==1); // respawn, ID is correlation only
    assert(prioritised.Id(6000,5000)==0);
    prioritised.World(701);
    assert(prioritised.Id(7777,8888,true)==1);
    assert(prioritised.Id(9000,8888)==2);

    // A render hook can observe the eventual local player first. The player
    // must still get ID 1 (and free the old NPC slot on promotion).
    Registry promoted;
    promoted.World(810);
    assert(promoted.Id(1111,2222)==2);
    const uint64_t old_serial=promoted.slots[1].serial;
    assert(old_serial>0);
    assert(promoted.Id(1111,2222,true)==1);
    assert(promoted.slots[1].address==0);
    const uint64_t local_serial=promoted.slots[0].serial;
    assert(local_serial>old_serial);
    assert(promoted.Id(1111,2222,false)==1);
    assert(promoted.Id(3333,2222)==2);
    assert(promoted.slots[1].serial>local_serial);
    assert(promoted.Id(1111,2222,true)==1);
    assert(promoted.slots[0].serial==local_serial); // repeated observation
    assert(promoted.Id(4444,2222,true)==1); // local character replaced
    assert(promoted.slots[0].serial>local_serial);
    // Local promotion must also work after the NPC registry is saturated.
    Registry packed;
    packed.World(820);
    for(uint32_t i=0;i<63;++i)
        assert(packed.Id(10000+i,24000)==i+2);
    assert(packed.Id(10040,24000,true)==1);
    assert(packed.slots[40+1].address==0);
    assert(packed.Id(10040,24000,false)==1);
    assert(packed.Id(99999,24000)==42);

    Budget warm;
    assert(warm.Attempt(1000,true));
    warm.Start(120000);
    assert(warm.Active(120001));

    Budget budget;
    budget.Start(1000);
    for(int i=0;i<256;++i) assert(budget.Attempt(1000,true));
    assert(!budget.Attempt(1000,true));
    for(int i=0;i<32;++i) assert(budget.Record(1000,true));
    assert(!budget.Record(1000,true));
    assert(budget.Attempt(2000,true) && budget.Record(2000,true));
    assert(!budget.Attempt(181000,true));
    Budget cap;
    cap.Start(0);
    for(uint32_t i=0;i<2048;++i) assert(cap.Record(uint64_t(i/32)*1000,true));
    assert(!cap.Record(65000,true));

    // LOD/animation floods must leave at least 64 player hook attempts and
    // 8 player records per second, plus 256 reserved lifetime records.
    Budget fair;
    fair.Start(1000);
    for(int i=0;i<192;++i) assert(fair.Attempt(1000,false));
    assert(!fair.Attempt(1000,false));
    for(int i=0;i<64;++i) assert(fair.Attempt(1000,true));
    assert(!fair.Attempt(1000,true));
    for(int i=0;i<24;++i) assert(fair.Record(1000,false));
    assert(!fair.Record(1000,false));
    for(int i=0;i<8;++i) assert(fair.Record(1000,true));
    assert(!fair.Record(1000,true));
    assert(fair.Attempt(2000,false) && fair.Record(2000,false));

    Budget lifetime;
    lifetime.Start(0);
    for(uint32_t i=0;i<1792;++i)
        assert(lifetime.Record(uint64_t(i/24)*1000,false));
    assert(!lifetime.Record(75000,false));
    for(uint32_t i=0;i<256;++i)
        assert(lifetime.Record(uint64_t(75+i/8)*1000,true));
    assert(lifetime.total==2048 && lifetime.nonplayer_total==1792);
    assert(!lifetime.Record(107000,true));

    ByteBudget bytes;
    assert(bytes.Reserve(1024*1024-10));
    assert(!bytes.Reserve(11));
    assert(bytes.Reserve(10));
    assert(!bytes.Reserve(1));

    Sample s;
    s.kind=Kind::Player;s.position_valid=true;s.x=1;s.y=2;s.z=3;
    s.id=1;s.epoch=1;
    char line[1024];
    auto n=Format(s,line,sizeof(line));
    assert(n>0 && line[n-1]=='\n');
    assert(n<=1024);
    assert(std::strstr(line,"\"position\":[1,2,3]")!=nullptr);
    s.x=std::numeric_limits<float>::quiet_NaN();
    assert(Format(s,line,sizeof(line))==0);
    s.x=1;
    assert(Format(s,line,8)==0);
    assert(!ValidPosition(1,2,std::numeric_limits<float>::infinity()));
    s.kind=Kind::Animation;
    std::memcpy(s.name.data(),"quote\"\n\\bad",11);
    n=Format(s,line,sizeof(line));
    assert(n>0 && std::strstr(line,"quote???bad")!=nullptr);
    std::cout<<line;
    std::cout<<"PASS guarded reads, bounds, IDs, limits and format\n";
}
