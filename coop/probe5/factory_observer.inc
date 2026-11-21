// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Included in bo_probe anonymous namespace; read-only, bounded, no guest calls.
struct FactoryTicket {bo_probe5::Entry entry{};uint32_t sequence=0,entry_error=6;bool sampled=false,valid=false;};
uint32_t factory_attempts=0;
bool Init(uint64_t now);
FactoryTicket FactoryBegin(uint32_t manager,uint32_t stack,const uint8_t* base)noexcept{
 FactoryTicket ticket;if(!enabled.load(std::memory_order_relaxed)||!base)return ticket;
 HostState preserve;
 try{
  std::unique_lock<std::mutex> held(lock,std::try_to_lock);
  if(!held.owns_lock()||!enabled.load(std::memory_order_relaxed)||factory_attempts>=32)return ticket;
  const uint64_t now=Now();if(!Init(now))return ticket;
  if(now-opened_at>=300000||(budget.started&&!budget.Active(now)))return ticket;
  ticket.sampled=true;ticket.sequence=++factory_attempts;
  GuestReader read(reinterpret_cast<uintptr_t>(base));uint32_t brain=0,descriptor=0;
  ticket.valid=read.Field(stack,4,brain)&&read.Field(stack,8,descriptor)&&bo_probe5::ReadEntry(read,manager,brain,descriptor,ticket.entry,&ticket.entry_error);
 }catch(...){ticket.valid=false;}
 return ticket;
}
void FactoryFinish(const FactoryTicket& ticket,uint32_t actor,const uint8_t* base)noexcept{
 if(!ticket.sampled||!base||!enabled.load(std::memory_order_relaxed))return;
 HostState preserve;
 try{
  std::unique_lock<std::mutex> held(lock,std::try_to_lock);
  if(!held.owns_lock()||!enabled.load(std::memory_order_relaxed)||output<0)return;
  GuestReader read(reinterpret_cast<uintptr_t>(base));bo_probe5::Result result;
  const bool verified=ticket.valid&&bo_probe5::ReadResult(read,ticket.entry,actor,result);
  const auto& e=ticket.entry;char line[768];
  int n=std::snprintf(line,sizeof(line),
   "{\"event\":\"factory\",\"sequence\":%u,\"entry_valid\":%s,\"entry_error\":%u,\"returned_null\":%s,\"links_verified\":%s,"
   "\"resource_index\":%u,\"controller_index\":%u,\"live_before\":%u,\"free_before\":%u,\"resource_flags_used\":%u,"
   "\"pose_finite\":%s,\"descriptor_kind\":%u,\"selector_a\":%u,\"selector_b\":%u,\"action_binding_present\":%s,"
   "\"slot\":%u,\"serial\":%u}\n",
   ticket.sequence,ticket.valid?"true":"false",ticket.entry_error,actor==0?"true":"false",verified?"true":"false",
   e.resource_index,e.controller_index,e.live,e.free,e.flags_used,e.pose_finite?"true":"false",e.kind,e.selector_a,e.selector_b,
   e.action_binding?"true":"false",result.slot,result.serial);
  if(n>0&&static_cast<size_t>(n)<sizeof(line))Write(line,static_cast<size_t>(n));
 }catch(...){/* Diagnostics never unwind across the original guest call. */}
}
