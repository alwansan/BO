"""Fail-closed isolated Probe3 transformation; never edits Probe2 implementation."""
import argparse
import hashlib
import json
import pathlib
import shutil
import tempfile
from prepare_probe import BASE_FILES,GAME_FILES,prepare as prepare_probe2,once
PACKAGE='org.bocoop.black.probe3'
HERE=pathlib.Path(__file__).resolve().parent

def prepare(root:pathlib.Path):
    root=root.resolve()
    originals={name:(root/name).read_bytes() for name in BASE_FILES+GAME_FILES}
    # Complete ALL source-anchor and addition validation in a disposable stage.
    with tempfile.TemporaryDirectory(prefix='bo-probe3-stage-') as d:
        stage=pathlib.Path(d)
        for name,data in originals.items():
            path=stage/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
        changes=prepare_probe2(stage,HERE/'probe')
        pending={name:(stage/name).read_text(encoding='utf-8') for name in changes}
    for name in pending:
        pending[name]=pending[name].replace('org.bocoop.black.probe2',PACKAGE).replace('BLACK Probe 2','BLACK Probe 3')
    for name in ('pool_snapshot.hpp','pool_observer.inc'):
        pending['src/coop/'+name]=(HERE/'probe3'/name).read_text(encoding='utf-8')
    cpp='src/coop/probe.cpp'
    text=pending[cpp]
    text=once(text,'#include "coop/probe_core.hpp"','#include "coop/probe_core.hpp"\n#include "coop/pool_snapshot.hpp"',cpp)
    text=once(text,'bool Init(uint64_t now) {','#include "pool_observer.inc"\nbool Init(uint64_t now) {',cpp)
    text=once(text,'void ObservePlayer(uint32_t object,const uint8_t* base) noexcept {Observe(Kind::Player,object,base);}',
        'void ObservePlayer(uint32_t object,const uint8_t* base) noexcept {Observe(Kind::Player,object,base);ObservePool(base);}',cpp)
    text=once(text,'void Enable() noexcept {',
        'void ObservePoolReset(uint32_t result) noexcept {\n'
        '    if(result==1 && enabled.load(std::memory_order_relaxed))\n'
        '        pool_reset_sequence.fetch_add(1,std::memory_order_acq_rel);\n'
        '}\nvoid Enable() noexcept {',cpp)
    text+='''
// Successful reset returns AL1; failed early exits must not count as resets.
// Context is passed solely to the original implementation, then read once.
X86_EXTERN_FUNC(__imp__sub_001D5E00);
X86_FUNC(sub_001D5E00) {
    __imp__sub_001D5E00(ctx,base);
    bo_probe::ObservePoolReset(ctx.eax & 0xFFu);
}
'''
    # Capture header states precisely what the new records prove.
    text=once(text,'world_epoch_pointer_vtable_correlations_not_proven_lifetimes',
        'pointer_epoch_plus_observed_successful_pool_resets_not_persistent_network_ids',cpp)
    pending[cpp]=text
    if len(pending)!=14:raise ValueError('Unexpected Probe3 file set')
    for name in pending:
        if name not in originals and (root/name).exists():
            raise ValueError('Refusing to replace preexisting addition: '+name)
    for name,text in pending.items():
        path=root/name;path.parent.mkdir(parents=True,exist_ok=True)
        path.write_text(text,encoding='utf-8',newline='\n')
    return {name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in pending}

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('root',type=pathlib.Path)
    args=parser.parse_args();print(json.dumps({'package':PACKAGE,'changed_files':prepare(args.root)},indent=2))
