"""Atomic opt-in factory observations on the automatic room build; never creates actors."""
import argparse,hashlib,json,pathlib,tempfile
from prepare_probe import once
from prepare_room_network import prepare as prepare_network,INPUTS
HERE=pathlib.Path(__file__).resolve().parent
PACKAGE='org.bocoop.black.factoryroom'
def prepare(root:pathlib.Path):
 root=root.resolve();originals={name:(root/name).read_bytes() for name in INPUTS}
 with tempfile.TemporaryDirectory(prefix='bo-factory-stage-') as directory:
  stage=pathlib.Path(directory)
  for name,data in originals.items():
   path=stage/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
  changed=prepare_network(stage)
  pending={name:(stage/name).read_text(encoding='utf-8') for name in changed}
 for name in pending:
  pending[name]=pending[name].replace('org.bocoop.black.roomnet',PACKAGE).replace('BLACK Co-op NETWORK TEST','BLACK Co-op FACTORY TEST')
 for name in ('factory_contract.hpp','factory_observer.inc'):
  pending['src/coop/'+name]=(HERE/'probe5'/name).read_text(encoding='utf-8')
 cpp='src/coop/probe.cpp';text=pending[cpp]
 text=once(text,'#include "coop/instance_graph.hpp"','#include "coop/instance_graph.hpp"\n#include "coop/factory_contract.hpp"',cpp)
 text=once(text,'#include "pool_observer.inc"','#include "pool_observer.inc"\n#include "factory_observer.inc"',cpp)
 text=once(text,'\\"max_records\\":2048','\\"max_records\\":2080,\\"factory_max_records\\":32',cpp)
 text+='''
// Entry EAX=manager, stack+4=independent controller, stack+8=descriptor.
// Observer keeps entry values on host stack; original executes exactly once.
X86_EXTERN_FUNC(__imp__sub_001DF440);
X86_FUNC(sub_001DF440) {
    const auto observation=bo_probe::FactoryBegin(ctx.eax,ctx.esp,base);
    __imp__sub_001DF440(ctx,base);
    bo_probe::FactoryFinish(observation,ctx.eax,base);
}
'''
 pending[cpp]=text
 java='app/src/main/java/com/racas/black/BlackActivity.java'
 pending[java]=pending[java].replace('Optional diagnostic capture of local character observations.','Optional diagnostic capture of original character creation and ownership.').replace('No network, actor creation or save changes. ','Room networking is separate; this diagnostic creates no actors and changes no saves. ')
 for name in pending:
  if name not in originals and (root/name).exists():raise ValueError('Refusing preexisting addition: '+name)
 for name,text in pending.items():
  path=root/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(text,encoding='utf-8',newline='\n')
 return {name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in pending}
if __name__=='__main__':
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('root',type=pathlib.Path)
 print(json.dumps({'package':PACKAGE,'changed_files':prepare(parser.parse_args().root)},indent=2))
