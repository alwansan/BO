"""Isolated opt-in instance graph probe on the owner c5 image; no guest writes."""
import argparse,hashlib,json,pathlib,tempfile
from prepare_probe import BASE_FILES,GAME_FILES,once
from prepare_probe3 import prepare as prepare_probe3
PACKAGE='org.bocoop.black.probe4'
HERE=pathlib.Path(__file__).resolve().parent

def prepare(root:pathlib.Path):
    root=root.resolve();originals={name:(root/name).read_bytes() for name in BASE_FILES+GAME_FILES}
    with tempfile.TemporaryDirectory(prefix='bo-probe4-stage-') as d:
        stage=pathlib.Path(d)
        for name,data in originals.items():
            path=stage/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
        changes=prepare_probe3(stage)
        pending={name:(stage/name).read_text(encoding='utf-8') for name in changes}
    for name in pending:
        pending[name]=pending[name].replace('org.bocoop.black.probe3',PACKAGE).replace('BLACK Probe 3','BLACK Probe 4')
    pending['src/coop/instance_graph.hpp']=(HERE/'probe4/instance_graph.hpp').read_text()
    pending['src/coop/pool_observer.inc']=(HERE/'probe4/pool_observer.inc').read_text()
    cpp='src/coop/probe.cpp'
    pending[cpp]=once(pending[cpp],'#include "coop/pool_snapshot.hpp"',
                      '#include "coop/pool_snapshot.hpp"\n#include "coop/instance_graph.hpp"',cpp)
    if len(pending)!=15:raise ValueError('Unexpected Probe4 output set')
    for name in pending:
        if name not in originals and (root/name).exists():raise ValueError('Refusing preexisting addition: '+name)
    for name,text in pending.items():
        path=root/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(text,encoding='utf-8',newline='\n')
    return {name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in pending}
if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('root',type=pathlib.Path)
    args=parser.parse_args();print(json.dumps({'package':PACKAGE,'changed_files':prepare(args.root)},indent=2))
