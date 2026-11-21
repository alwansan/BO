# Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
"""Strict Probe3 pool reader producing aggregates only, never raw guest identities."""
import argparse
from collections import Counter
import json
from pathlib import Path


def integer(value, maximum=(1<<32)-1):
    if type(value) is not int or not 0<=value<=maximum:
        raise ValueError('Invalid unsigned integer in pool record')
    return value


def analyze(path:Path):
    with Path(path).open('rb') as source:
        raw=source.read(1048577)
    if len(raw)>1048576:raise ValueError('Capture exceeds native byte budget')
    counts=Counter();transitions=Counter();states=Counter();scopes=set();generations=set()
    previous=None;last_scope=None;first_ms=None;last_ms=None;header=False;ended=False
    serial_changes=scope_changes=counter_drops=resource_changes=state_mismatches=0
    max_resource320=max_resource35c=shared320=shared35c=0
    pool_counts={'total':0,'valid':0,'invalid':0}
    last_attempt=last_rejected=0
    for index,line in enumerate(raw.splitlines(),1):
        if not line.strip():continue
        if index>2050 or len(line)>8192:raise ValueError('Capture record budget exceeded')
        try:record=json.loads(line)
        except (ValueError,UnicodeError):raise ValueError('Invalid JSON record') from None
        if not isinstance(record,dict) or ended:raise ValueError('Invalid record or data after end')
        event=record.get('event')
        if not header:
            if event!='start' or record.get('probe')!='HOOK001' or record.get('scope')!='read_only_no_multiplayer' or record.get('reader')!='self_process_vm_readv':
                raise ValueError('Expected read-only Probe3-compatible header')
            header=True;counts['start']+=1;continue
        if event not in ('player','lod','animation','pool','end'):raise ValueError('Unexpected event')
        counts[event]+=1
        if event=='end':ended=True;continue
        if event!='pool':continue # Legacy private positions/thread IDs are deliberately ignored.
        ms=integer(record.get('ms'),180000)
        epoch=integer(record.get('world_epoch'))
        reset=integer(record.get('reset_sequence'),(1<<64)-1)
        scope=(epoch,reset)
        if last_ms is not None and ms<last_ms:raise ValueError('Pool time went backwards')
        if last_scope is not None and (epoch<last_scope[0] or reset<last_scope[1]):raise ValueError('Pool scope went backwards')
        if first_ms is None:first_ms=ms
        last_ms=ms;scopes.add(scope)
        if last_scope is not None and scope!=last_scope:scope_changes+=1
        last_scope=scope
        attempts=integer(record.get('attempts'),2048);rejected=integer(record.get('rejected'),2048)
        if attempts<last_attempt or rejected<last_rejected or rejected>attempts:raise ValueError('Invalid sampling counters')
        last_attempt=attempts;last_rejected=rejected
        valid=record.get('valid')
        if type(valid) is not bool:raise ValueError('Pool valid flag must be boolean')
        pool_counts['total']+=1
        if not valid:
            pool_counts['invalid']+=1;previous=None;continue
        pool_counts['valid']+=1
        counter=integer(record.get('counter'))
        occupied=record.get('occupied');actors=record.get('actors')
        if not isinstance(occupied,list) or len(occupied)!=4 or not isinstance(actors,list) or len(actors)!=32:
            raise ValueError('Wrong pool dimensions')
        for value,cap in zip(occupied,(32,16,8,8)):
            integer(value,cap)
        if sum(occupied)!=32:raise ValueError('Missing pool membership')
        physical=Counter();r320=set();r35c=set();active=0
        for slot,actor in enumerate(actors):
            if not isinstance(actor,list) or len(actor)!=6:raise ValueError('Wrong actor tuple')
            for value in actor:integer(value)
            if actor[0]!=slot or actor[1]>3:raise ValueError('Wrong or duplicate slot')
            group,serial,state,resource320,resource35c=actor[1:]
            physical[group]+=1
            if not group:
                if any(actor[2:]):raise ValueError('Free slot has stale public metadata')
                continue
            active+=1
            if state!=group-1:state_mismatches+=1 # Observation, not a semantic rejection.
            generations.add((epoch,reset,slot,serial))
            if resource320:r320.add(resource320)
            if resource35c:r35c.add(resource35c)
        if [physical[i] for i in range(4)]!=occupied:raise ValueError('Occupied count mismatch')
        max_resource320=max(max_resource320,len(r320));max_resource35c=max(max_resource35c,len(r35c))
        if active>len(r320):shared320+=1
        if active>len(r35c):shared35c+=1
        if previous is not None and previous[0]==scope:
            old_counter,old_actors=previous[1:]
            if counter<old_counter:counter_drops+=1
            for old,new in zip(old_actors,actors):
                if old[1]!=new[1]:transitions[f'{old[1]}->{new[1]}']+=1
                if old[1] and new[1]:
                    states[f'{old[3]}->{new[3]}']+=1
                    if old[2]!=new[2]:
                        serial_changes+=1
                        if old[4:]!=new[4:]:resource_changes+=1
        previous=(scope,counter,actors)
    if not header or not pool_counts['total']:raise ValueError('No Probe3 pool observations')
    return {'schema':1,'events':dict(sorted(counts.items())),'pool':pool_counts,
            'captureComplete':ended,'durationMs':(last_ms-first_ms),'scopeCount':len(scopes),'scopeChanges':scope_changes,
            'groupTransitions':dict(sorted(transitions.items())),'stateTransitions':dict(sorted(states.items())),
            'occupiedSerialChanges':serial_changes,'resourceChangesWithSerialChange':resource_changes,
            'observedSlotGenerations':len(generations),'counterDecreasesWithinScope':counter_drops,
            'groupStateMismatches':state_mismatches,'maxDistinctResource320PerSnapshot':max_resource320,
            'maxDistinctResource35cPerSnapshot':max_resource35c,'snapshotsWithSharedOrZeroResource320':shared320,
            'snapshotsWithSharedOrZeroResource35c':shared35c,'provesPlayableCoop':False,
            'limitations':['sampled observations, not exact spawn/death callbacks','no atomicity or ABA proof',
                           'resource pointers are not model-instance ownership proof','raw identities and private positions omitted']}

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('capture',type=Path)
    args=parser.parse_args();print(json.dumps(analyze(args.capture),indent=2,ensure_ascii=False))
