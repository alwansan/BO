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

    Budget warm;
    assert(warm.Attempt(1000));
    warm.Start(120000);
    assert(warm.Active(120001));

    Budget budget;
    budget.Start(1000);
    for(int i=0;i<256;++i) assert(budget.Attempt(1000));
    assert(!budget.Attempt(1000));
    for(int i=0;i<32;++i) assert(budget.Record(1000));
    assert(!budget.Record(1000));
    assert(budget.Attempt(2000) && budget.Record(2000));
    assert(!budget.Attempt(181000));
    Budget cap;
    cap.Start(0);
    for(uint32_t i=0;i<2048;++i) assert(cap.Record(uint64_t(i/32)*1000));
    assert(!cap.Record(65000));

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
