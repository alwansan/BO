import os,pathlib,shutil,tempfile,unittest
from prepare_room_player import INPUTS,PACKAGE,prepare
class OriginalPlayerStageTests(unittest.TestCase):
 def test_only_existing_player_hook_and_no_factory_or_replacement(self):
  with tempfile.TemporaryDirectory() as directory:
   root=pathlib.Path(directory)
   for name in INPUTS:
    p=root/name;p.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(pathlib.Path(os.environ['BO_UPSTREAM_REFERENCE'])/name,p)
   prepare(root);cpp=(root/'src/coop/probe.cpp').read_text(encoding='utf-8')
   self.assertIn('ObserveDefaultModel(object,base)',cpp);self.assertNotIn('sub_001DF440',cpp)
   self.assertFalse((root/'src/coop/factory_contract.hpp').exists())
   self.assertIn(PACKAGE,(root/'app/build.gradle').read_text(encoding='utf-8'))
   before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
   with self.assertRaises(ValueError):prepare(root)
   self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
