#include "room_original_host_remote_pad_plan.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using namespace blackcoop;
int main(){
 OriginalHostHumanInbox inbox;OriginalHostRemotePadValue out{};
 const int32_t all[]={42,3,2,10,1000,-1000,1200,3,
                       3,11,-750,250,-1200,1,
                       4,12,0,0,-18000,2};
 assert(inbox.Ingest(all,sizeof(all)/sizeof(all[0]))==3);
 assert(!OriginalHostRemotePadPlan::Preview(inbox,42,0,true,true,out));
 assert(!OriginalHostRemotePadPlan::Preview(inbox,42,4,true,true,out));
 assert(!OriginalHostRemotePadPlan::Preview(inbox,41,1,true,true,out));
 assert(!OriginalHostRemotePadPlan::Preview(inbox,42,1,false,true,out));
 assert(!OriginalHostRemotePadPlan::Preview(inbox,42,1,true,false,out));
 assert(!out.connected&&out.analog[7]==0);
 assert(OriginalHostRemotePadPlan::Preview(inbox,42,1,true,true,out));
 assert(out.connected&&out.port==1&&out.lx==32767&&out.ly==-32768);
 assert(out.analog[0]==255&&out.analog[7]==255&&out.buttons==0);
 assert(out.epoch==42&&out.sequence==10&&out.yawCentiDegrees==1200);
 assert(out.rx==0&&out.ry==0);
 assert(OriginalHostRemotePadPlan::Preview(inbox,42,2,true,true,out));
 assert(out.port==2&&out.lx< -24000&&out.ly>8000);
 assert(out.analog[0]==0&&out.analog[7]==255&&out.sequence==11);
 assert(OriginalHostRemotePadPlan::Preview(inbox,42,3,true,true,out));
 assert(out.port==3&&out.lx==0&&out.ly==0);
 assert(out.analog[0]==255&&out.analog[7]==0);
 bool foreignAccepted=true;
 std::thread worker([&]{OriginalHostRemotePadValue other{};
  foreignAccepted=OriginalHostRemotePadPlan::Preview(inbox,42,1,true,true,other);
  assert(!foreignAccepted&&!other.connected);});
 worker.join();assert(!foreignAccepted);
 const int32_t idle[]={42,0};
 for(int i=0;i<4;i++)assert(inbox.Ingest(idle,2)==0);
 assert(!OriginalHostRemotePadPlan::Preview(inbox,42,1,true,true,out));
 assert(!out.connected&&out.lx==0&&out.analog[7]==0);
 const int32_t next[]={43,1,2,1,-100,100,0,1};
 assert(inbox.Ingest(next,8)==1);
 assert(!OriginalHostRemotePadPlan::Preview(inbox,42,1,true,true,out));
 assert(OriginalHostRemotePadPlan::Preview(inbox,43,1,true,true,out));
 assert(out.port==1&&out.sequence==1&&out.analog[7]==255);
 assert(!OriginalHostRemotePadPlan::Preview(inbox,43,2,true,true,out));
 const int32_t leave[]={0,0};assert(inbox.Ingest(leave,2)==0);
 assert(!OriginalHostRemotePadPlan::Preview(inbox,43,1,true,true,out));
 assert(!out.connected);
 std::cout<<"ORIGINAL_HOST_TLS_PAD_PORTS_1_2_3_READONLY_PASS\n";
}
