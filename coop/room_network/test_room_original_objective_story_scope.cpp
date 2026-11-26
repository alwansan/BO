// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Synthetic room identities, descriptor IDs and progress values only.
#include "room_original_objective_story_scope.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
using namespace blackcoop;
namespace {
constexpr OriginalObjectiveDescriptorId kFirst{0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x80};
constexpr OriginalObjectiveDescriptorId kSecond{0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0xff};
constexpr std::array<OriginalObjectiveDescriptorId,2> kExpected{kFirst,kSecond};

OriginalObjectiveStoryScope Scope(){
 OriginalObjectiveStoryScope scope;
 for(size_t i=0;i<scope.session.size();++i)scope.session[i]=static_cast<uint8_t>(i+1u);
 scope.token=17;scope.rosterRevision=6;scope.mission=0;scope.stage=1;
 scope.sceneEpoch=23;scope.participants=3;
 return scope;
}
std::vector<OriginalObjectiveValue> Values(uint32_t counter){
 return {{kFirst,counter,1u},{kSecond,9u,2u}};
}

void TestMatchingOwnedProgress(){
 auto scope=Scope();const auto acceptedScope=scope;
 auto source=Values(7u);const auto original=source;
 auto expected=kExpected;
 OriginalObjectiveStorySnapshot firstSnapshot,secondSnapshot;
 assert(CaptureOriginalObjectiveStorySnapshot(scope,source,firstSnapshot));
 assert(source==original&&firstSnapshot.scope==acceptedScope);
 PreparedOriginalObjectiveStory first,latest;
 assert(PrepareOriginalObjectiveStorySnapshot(firstSnapshot,scope,expected,first));
 assert(first.scope==acceptedScope&&first.values==original);
 source[0].counter=8u; // progress changes without changing the objective state
 assert(CaptureOriginalObjectiveStorySnapshot(scope,source,secondSnapshot));
 assert(PrepareOriginalObjectiveStorySnapshot(secondSnapshot,scope,expected,latest));
 assert(latest.scope==acceptedScope&&latest.values==source);
 assert(first.values[0].counter==7u&&latest.values[0].counter==8u);
 assert(first.values[0].state==latest.values[0].state&&
        first.values[0].descriptorId==latest.values[0].descriptorId);
 // Scope matching deliberately supplies no ordering within one ticket.
 auto olderAgain=latest;
 assert(PrepareOriginalObjectiveStorySnapshot(firstSnapshot,scope,expected,olderAgain));
 assert(olderAgain.scope==acceptedScope&&olderAgain.values==original);
 source.clear();expected[0][0]^=0x80;scope.session[0]^=0xff;
 std::fill(firstSnapshot.records.begin(),firstSnapshot.records.end(),0);
 std::fill(secondSnapshot.records.begin(),secondSnapshot.records.end(),0);
 assert(first.scope==acceptedScope&&first.values==original);
 assert(latest.scope==acceptedScope&&latest.values==Values(8u));
 first.values[0].counter=0;
 assert(latest.values[0].counter==8u&&olderAgain.values[0].counter==7u);
}

void TestAllSevenScopeFields(){
 const auto current=Scope();
 const PreparedOriginalObjectiveStory sentinel{current,Values(7u)};
 const auto changedValues=Values(41u);
 std::array<OriginalObjectiveStoryScope,7> mismatches;
 mismatches.fill(current);
 mismatches[0].session[15]^=1u;
 ++mismatches[1].token;
 ++mismatches[2].rosterRevision;
 ++mismatches[3].mission;
 ++mismatches[4].stage;
 ++mismatches[5].sceneEpoch;
 mismatches[6].participants=7;
 for(const auto& mismatch:mismatches){
  assert(ValidOriginalObjectiveStoryScope(mismatch)&&!(mismatch==current));
  OriginalObjectiveStorySnapshot snapshot;
  assert(CaptureOriginalObjectiveStorySnapshot(mismatch,changedValues,snapshot));
  // Independently confirm that only the scope can cause this rejection.
  std::vector<OriginalObjectiveValue> decoded;
  assert(PrepareOriginalObjectiveValues(snapshot.records,kExpected,decoded));
  assert(decoded==changedValues&&decoded!=sentinel.values);
  auto output=sentinel;
  assert(!PrepareOriginalObjectiveStorySnapshot(snapshot,current,kExpected,output));
  assert(output==sentinel);
 }
}

void TestTicketTransition(){
 const auto previous=Scope();
 auto next=previous;
 ++next.token;++next.rosterRevision;++next.mission;++next.stage;++next.sceneEpoch;
 next.participants=7;
 const auto previousValues=Values(7u),nextValues=Values(21u);
 OriginalObjectiveStorySnapshot previousSnapshot,nextSnapshot;
 assert(CaptureOriginalObjectiveStorySnapshot(previous,previousValues,previousSnapshot));
 assert(CaptureOriginalObjectiveStorySnapshot(next,nextValues,nextSnapshot));
 PreparedOriginalObjectiveStory output;
 assert(PrepareOriginalObjectiveStorySnapshot(previousSnapshot,previous,kExpected,output));
 const auto before=output;
 assert(!PrepareOriginalObjectiveStorySnapshot(nextSnapshot,previous,kExpected,output));
 assert(output==before);
 // The local current ticket changes independently of the incoming snapshot.
 const auto current=next;
 assert(PrepareOriginalObjectiveStorySnapshot(nextSnapshot,current,kExpected,output));
 assert(output.scope==current&&output.values==nextValues);
 const auto accepted=output;
 assert(!PrepareOriginalObjectiveStorySnapshot(previousSnapshot,current,kExpected,output));
 assert(output==accepted); // identical descriptor IDs cannot authorize an old ticket
}

void TestTicketBounds(){
 auto wide=Scope();wide.session.fill(0);
 wide.token=std::numeric_limits<int64_t>::max();
 wide.rosterRevision=0xffffffffLL;wide.mission=7;wide.stage=65535;
 wide.sceneEpoch=std::numeric_limits<int32_t>::max();wide.participants=15;
 const auto values=Values(65535u);
 assert(ValidOriginalObjectiveStoryScope(wide));
 OriginalObjectiveStorySnapshot snapshot;
 assert(CaptureOriginalObjectiveStorySnapshot(wide,values,snapshot));
 PreparedOriginalObjectiveStory accepted;
 assert(PrepareOriginalObjectiveStorySnapshot(snapshot,wide,kExpected,accepted));
 assert(accepted.scope==wide&&accepted.values==values);
 // Ticket permits sparse guest masks too; do not narrow it to 3/7/15.
 for(int32_t participants=3;participants<=15;participants+=2){
  auto sparse=wide;sparse.participants=participants;
  assert(ValidOriginalObjectiveStoryScope(sparse));
  OriginalObjectiveStorySnapshot candidate;
  PreparedOriginalObjectiveStory prepared;
  assert(CaptureOriginalObjectiveStorySnapshot(sparse,values,candidate));
  assert(PrepareOriginalObjectiveStorySnapshot(candidate,sparse,kExpected,prepared));
  assert(prepared.scope==sparse&&prepared.values==values);
 }
 std::array<OriginalObjectiveStoryScope,16> invalid;
 invalid.fill(wide);
 invalid[0].token=0;invalid[1].token=-1;
 invalid[2].rosterRevision=0;invalid[3].rosterRevision=-1;
 invalid[4].rosterRevision=0x100000000LL;
 invalid[5].mission=-1;invalid[6].mission=8;
 invalid[7].stage=-1;invalid[8].stage=65536;
 invalid[9].sceneEpoch=0;invalid[10].sceneEpoch=-1;
 invalid[11].participants=2;invalid[12].participants=16;
 invalid[13].participants=4;invalid[14].participants=-1;
 invalid[15].participants=0;
 for(const auto& scope:invalid){
  assert(!ValidOriginalObjectiveStoryScope(scope));
  auto unchangedSnapshot=snapshot;
  assert(!CaptureOriginalObjectiveStorySnapshot(scope,values,unchangedSnapshot));
  assert(unchangedSnapshot==snapshot);
  auto invalidSnapshot=snapshot;invalidSnapshot.scope=scope;
  auto output=accepted;
  // Equal invalid scopes must fail validation, not pass the equality test.
  assert(!PrepareOriginalObjectiveStorySnapshot(invalidSnapshot,scope,kExpected,output));
  assert(output==accepted);
 }
}

void TestPayloadFailurePublishesNothing(){
 const auto previous=Scope();auto current=previous;
 ++current.rosterRevision;++current.token;
 const auto values=Values(17u);
 const PreparedOriginalObjectiveStory sentinel{previous,Values(42u)};
 OriginalObjectiveStorySnapshot good;
 assert(CaptureOriginalObjectiveStorySnapshot(current,values,good));
 const auto rejected=[&](const OriginalObjectiveStorySnapshot& snapshot,
                         std::span<const OriginalObjectiveDescriptorId> expected){
  auto output=sentinel;
  assert(snapshot.scope==current&&ValidOriginalObjectiveStoryScope(current));
  assert(!PrepareOriginalObjectiveStorySnapshot(snapshot,current,expected,output));
  assert(output==sentinel); // neither scope nor values may be published early
 };
 auto malformed=good;malformed.records.pop_back();rejected(malformed,kExpected);
 malformed=good;malformed.records.push_back(0);rejected(malformed,kExpected);
 malformed=good;malformed.records[26]=1;rejected(malformed,kExpected); // late counter
 malformed=good;malformed.records[28]=4;rejected(malformed,kExpected); // late state
 auto wrongIds=kExpected;wrongIds[1][7]^=1u;rejected(good,wrongIds);
 rejected(good,std::span(kExpected).first(1));
 // Capture also preserves both parts if a later value cannot be encoded.
 OriginalObjectiveStorySnapshot output;
 assert(CaptureOriginalObjectiveStorySnapshot(previous,Values(42u),output));
 const auto before=output;
 auto invalidValues=values;invalidValues[1].counter=65536u;
 assert(!CaptureOriginalObjectiveStorySnapshot(current,invalidValues,output));
 assert(output==before);
}
} // namespace

int main(){
 TestMatchingOwnedProgress();TestAllSevenScopeFields();TestTicketTransition();
 TestTicketBounds();TestPayloadFailurePublishesNothing();
 std::cout<<"ORIGINAL_OBJECTIVE_STORY_SCOPE_PASS scenarios=5 mismatch_fields=7 NO_ENGINE_OR_TRANSPORT\n";
}
