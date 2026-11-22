#include "room_original_ai_target_contract.hpp"
#include <array>
#include <cassert>
#include <iostream>
int main(){
 using namespace blackcoop;
 std::array<OriginalAiTargetRecord,32> table{};
 for(auto& v:table)v={5u,0u};
 const uint32_t player=0x123450u;
 table[5]={0u,player};
 auto all=CountOriginalAiTargets(table,player,5);
 assert(all.free==31&&all.inUse==1&&all.unknown==0&&
        all.localOriginalPlayerBound&&all.freeCode()==99);
 assert(!CountOriginalAiTargets(table,player,6).localOriginalPlayerBound);
 table[5]={0u,player+4u};
 assert(!CountOriginalAiTargets(table,player,5).localOriginalPlayerBound);
 for(auto& v:table)v={0u,0x1234u};
 auto none=CountOriginalAiTargets(table,player,5);
 assert(none.free==0&&none.inUse==32&&none.freeCode()==96);
 table[0]={5u,0u};
 assert(CountOriginalAiTargets(table,player,5).freeCode()==97);
 table[1]={5u,0u};
 assert(CountOriginalAiTargets(table,player,5).freeCode()==98);
 table[2]={5u,0u};
 assert(CountOriginalAiTargets(table,player,5).freeCode()==99);
 table[3]={7u,0u};
 auto unknown=CountOriginalAiTargets(table,player,5);
 assert(unknown.unknown==1);
 table[4]={0u,0u};
 assert(CountOriginalAiTargets(table,player,5).unknown==2);
 std::cout<<"ORIGINAL_AI_TARGET_POOL_READ_ONLY_PASS: 32 entries, free/live/unknown, original actor index binding; NO AI or actor writes"<<std::endl;
}
