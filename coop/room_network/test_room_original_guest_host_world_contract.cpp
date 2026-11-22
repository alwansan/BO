#include "room_original_guest_host_world_contract.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <thread>
using blackcoop::OriginalGuestHostWorldInbox;
using blackcoop::OriginalHostAvatar;
int main(){
    OriginalGuestHostWorldInbox guest;
    OriginalHostAvatar remote{};
    const int32_t valid[]={17,2,3,3,
       1,100,200,300,1000,0,
       2,120,200,300,1000,1,
       3,140,200,300,1000,2};
    const int32_t clear[]={0,0,0,0};
    const int32_t invalid={0};
    assert(!guest.ReadRemote(1,1000,remote));
    assert(guest.Ingest(valid,22,1000));
    assert(guest.Scene()==17);
    assert(guest.ReadRemote(1,1000,remote)&&remote.x==100&&remote.y==200);
    assert(guest.ReadRemote(2,1010,remote)&&remote.flags==1);
    assert(!guest.ReadRemote(3,1000,remote)); // original first-person self
    assert(!guest.ReadRemote(1,1351,remote)); // 350ms TTL
    assert(!guest.Ingest(valid,22,1100)); // replay same tick cannot refresh
    assert(!guest.ReadRemote(1,1451,remote));
    // An invalid duplicate or missing host must never overwrite a snapshot.
    int32_t corrupt[22];for(int i=0;i<22;++i)corrupt[i]=valid[i];
    corrupt[16]=2; // duplicate actor two
    assert(!guest.Ingest(corrupt,22,1150));
    corrupt[16]=3;
    corrupt[4]=4; // host absent and out-of-order
    assert(!guest.Ingest(corrupt,22,1150));
    corrupt[4]=1;
    corrupt[13]=21000000; // bad cm
    assert(!guest.Ingest(corrupt,22,1150));
    // Successful later authoritative host snapshot restores freshness.
    corrupt[13]=200;
    corrupt[1]=3;
    assert(guest.Ingest(corrupt,22,1200));
    assert(guest.ReadRemote(1,1200,remote));
    bool foreign=false;
    std::thread worker([&]{foreign=guest.Ingest(valid,22,1500);
                           assert(!guest.ReadRemote(1,1500,remote));
                           assert(!guest.Clear());});
    worker.join();
    assert(!foreign&&guest.ReadRemote(1,1300,remote));
    // A new mission epoch may start at tick one: never retain an old human.
    int32_t next[22];for(int i=0;i<22;i++)next[i]=valid[i];
    next[0]=18;next[1]=1;
    assert(guest.Ingest(next,22,1400));
    assert(guest.Scene()==18);
    assert(guest.ReadRemote(2,1400,remote));
    assert(!guest.Ingest(&invalid,1,1500));
    assert(!guest.Ingest(valid,22,0));
    assert(!guest.Ingest(clear,4,1500));
    assert(guest.Scene()==0&&!guest.ReadRemote(1,1500,remote));
    std::cout<<"GUEST_ORIGINAL_HOST_WORLD_INBOX_PASS: trusted host snapshot, 3 remote humans, TTL, epoch/identity, no original actor writes\n";
}
