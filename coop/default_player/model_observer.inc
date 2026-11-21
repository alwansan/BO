// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original local player only. No new actor/model, guest write or guest call.
uint32_t default_model_records=0;uint64_t default_model_bucket=UINT64_MAX;
bool Init(uint64_t now);
void ObserveDefaultModel(uint32_t actor,const uint8_t* base)noexcept{
 if(!base||!enabled.load(std::memory_order_relaxed))return;
 HostState preserve;
 try{
  std::unique_lock<std::mutex> held(lock,std::try_to_lock);
  if(!held.owns_lock()||!enabled.load(std::memory_order_relaxed)||default_model_records>=8)return;
  const uint64_t now=Now();if(default_model_bucket==now/1000||!Init(now))return;
  if(now-opened_at>=300000||(budget.started&&!budget.Active(now)))return;
  default_model_bucket=now/1000;++default_model_records;
  GuestReader read(reinterpret_cast<uintptr_t>(base));bo_player::ModelInfo info;
  const bool valid=bo_player::ReadModel(read,actor,info);char line[512];
  int n=std::snprintf(line,sizeof(line),
   "{\"event\":\"default_player_model\",\"valid\":%s,\"has_model\":%s,\"complete\":%s,\"bone_count\":%u,\"buffer_count\":%u,"
   "\"draw_list_mask\":%u,\"head\":%s,\"torso\":%s,\"legs\":%s,\"arms\":%s,\"body_candidate\":%s}\n",
   valid?"true":"false",info.has_model?"true":"false",info.complete?"true":"false",info.bones,info.buffers,info.draw_lists,
   info.head?"true":"false",info.torso?"true":"false",info.legs?"true":"false",info.arms?"true":"false",
   valid&&info.complete&&info.head&&info.torso&&info.legs?"true":"false");
  if(n>0&&static_cast<size_t>(n)<sizeof(line))Write(line,static_cast<size_t>(n));
 }catch(...){/* Leave the original player's execution untouched. */}
}
