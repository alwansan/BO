import pathlib,unittest,tempfile,os,shutil
from prepare_room_factory import prepare,INPUTS,PACKAGE
class FactoryPrepareTests(unittest.TestCase):
 def source(self,root):
  for name in INPUTS:
   p=root/name;p.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(pathlib.Path(os.environ['BO_UPSTREAM_REFERENCE'])/name,p)
 def test_original_called_once_and_separate_atomic_stage(self):
  with tempfile.TemporaryDirectory() as directory:
   root=pathlib.Path(directory);self.source(root);changed=prepare(root)
   self.assertGreaterEqual(len(changed),30)
   cpp=(root/'src/coop/probe.cpp').read_text(encoding='utf-8')
   self.assertEqual(cpp.count('__imp__sub_001DF440(ctx,base);'),1)
   self.assertIn('FactoryBegin(ctx.eax,ctx.esp,base)',cpp);self.assertIn('FactoryFinish(observation,ctx.eax,base)',cpp)
   self.assertIn('factory_max_records',cpp)
   self.assertIn(PACKAGE,(root/'app/build.gradle').read_text(encoding='utf-8'))
   before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
   with self.assertRaises(ValueError):prepare(root)
   self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
 def test_existing_addition_refused_before_writes(self):
  with tempfile.TemporaryDirectory() as directory:
   root=pathlib.Path(directory);self.source(root);p=root/'src/coop/factory_contract.hpp';p.parent.mkdir(parents=True);p.write_bytes(b'owner local file')
   before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
   with self.assertRaises(ValueError):prepare(root)
   self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
