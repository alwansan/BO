"""Original protagonist model observations, no NPC model substitution or factory hook."""
import argparse,hashlib,json,pathlib,tempfile
from prepare_probe import once
from prepare_room_network import prepare as prepare_network,INPUTS
HERE=pathlib.Path(__file__).resolve().parent
PACKAGE='org.bocoop.black.defaultplayer'
def prepare(root:pathlib.Path):
 root=root.resolve();originals={name:(root/name).read_bytes() for name in INPUTS}
 with tempfile.TemporaryDirectory(prefix='bo-original-player-') as directory:
  stage=pathlib.Path(directory)
  for name,data in originals.items():
   p=stage/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
  changed=prepare_network(stage);pending={name:(stage/name).read_text(encoding='utf-8') for name in changed}
 for name in pending:
  pending[name]=pending[name].replace('org.bocoop.black.roomnet',PACKAGE).replace('BLACK Co-op NETWORK TEST','BLACK ORIGINAL PLAYER TEST')
 for name in ('model_contract.hpp','model_observer.inc'):
  pending['src/coop/'+name]=(HERE/'default_player'/name).read_text(encoding='utf-8')
 cpp='src/coop/probe.cpp';s=pending[cpp]
 s=once(s,'#include "coop/instance_graph.hpp"','#include "coop/instance_graph.hpp"\n#include "coop/model_contract.hpp"',cpp)
 s=once(s,'#include "pool_observer.inc"','#include "pool_observer.inc"\n#include "model_observer.inc"',cpp)
 s=once(s,'Observe(Kind::Player,object,base);ObservePool(base);','Observe(Kind::Player,object,base);ObservePool(base);ObserveDefaultModel(object,base);',cpp)
 s=once(s,'\\"max_records\\":2048','\\"max_records\\":2056,\\"default_model_max_records\\":8',cpp)
 pending[cpp]=s
 java='app/src/main/java/com/racas/black/BlackActivity.java'
 pending[java]=pending[java].replace('Optional diagnostic capture of local character observations.','Optional diagnostic capture of the ORIGINAL protagonist model; no NPC or ally replacement.').replace('No network, actor creation or save changes. ','Room networking is separate; this capture creates no actors and changes no saves. ')
 for name in pending:
  if name not in originals and (root/name).exists():raise ValueError('Refusing preexisting addition: '+name)
 for name,text in pending.items():
  p=root/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text,encoding='utf-8',newline='\n')
 return {name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in pending}
if __name__=='__main__':
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('root',type=pathlib.Path)
 print(json.dumps({'package':PACKAGE,'changed_files':prepare(parser.parse_args().root)},indent=2))
