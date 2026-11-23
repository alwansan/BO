// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "room_original_host_proxy_engine.hpp"
#include "cpu/x86_context.h"
#include "kernel/kernel.h"
#include "kernel/memory.h"
namespace dispatch { X86Func* Find(uint32_t); }
namespace blackcoop {
// Thin REAL engine bridge. The caller supplies the current original tick's
// context and a live committed-host-scene accessor, not a cached network epoch.
// Construct and use only on the original game thread. Never auto-creates actors.
class OriginalHostProxyKernelIO {
 X86Context& context_;
 uint64_t (*scene_)()noexcept;
public:
 OriginalHostProxyKernelIO(X86Context& context,uint64_t (*scene)()noexcept)
  :context_(context),scene_(scene){}
 uint64_t Scene()const noexcept{return scene_?scene_():0;}
 // The generic kernel bridge cannot prove a second actor's allocation.
 // Only an explicit tracked original guest storage IO may permit it.
 bool OwnsActor(uint32_t,uint32_t)const noexcept{return false;}
 bool Span(uint32_t address,uint32_t size)const noexcept{
  if(address<0x10000u||!size||uint64_t(address)+size>0x100000000ull)return false;
  const uint64_t end=(uint64_t(address)+size-1u)&~uint64_t{0xFFF};
  for(uint64_t page=uint64_t(address)&~uint64_t{0xFFF};page<=end;page+=0x1000u)
   if(!mem::IsCommitted(static_cast<uint32_t>(page)))return false;
  return true;
 }
 bool Read32(uint32_t address,uint32_t& value)const noexcept{
  if(!Span(address,4))return false;
  value=kernel::R32(address);return true;
 }
 bool Read8(uint32_t address,uint8_t& value)const noexcept{
  if(!Span(address,1))return false;
  value=kernel::G<uint8_t>(address);return true;
 }
 bool Call(uint32_t function,uint32_t physics,uint32_t argument,uint32_t& result){
  if((function!=0xDF400u&&function!=0xDBBE0u)||!dispatch::Find(function))return false;
  X86Context call=context_; // isolate integer AND SIMD/x87 context mutations
  call.eax=call.ecx=call.edx=call.esi=call.edi=0;
  call.ebx=physics;
  result=kernel::CallGuest(call,function,{argument});
  return true;
 }
};
}
