import os,pathlib,shutil,tempfile,unittest
from prepare_probe import BASE_FILES,GAME_FILES
from prepare_room_ui import prepare,PACKAGE
class RoomUIPrepareTests(unittest.TestCase):
 def fixture(self,root):
  src=pathlib.Path(os.environ['BO_UPSTREAM_REFERENCE'])
  for name in BASE_FILES+GAME_FILES+('src/game/exit_menu.cpp',):
   p=root/name;p.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src/name,p)
 def test_in_game_title_route_and_isolated_package(self):
  with tempfile.TemporaryDirectory() as d:
   root=pathlib.Path(d);self.fixture(root);changed=prepare(root)
   self.assertEqual(len(changed),18)
   self.assertEqual(PACKAGE,'org.bocoop.black.roomui')
   self.assertIn('applicationId "'+PACKAGE+'"',(root/'app/build.gradle').read_text())
   title=(root/'src/game/exit_menu.cpp').read_text()
   self.assertIn('"CoopButton"',title)
   self.assertIn('flash::Link(ctx, options, coop);',title)
   self.assertIn('flash::Link(ctx, coop, last == options ? button : last);',title)
   self.assertIn('game::RequestCoopRoom',title)
   opts=(root/'src/game/options_menu.cpp').read_text()
   self.assertIn('g_room_route.Take(blackcoop::Page::Options)',opts)
   self.assertIn('p.return_title',opts)
   self.assertIn('case -102: return "NOT AVAILABLE";',opts)
   self.assertIn('case -103: return "NOT AVAILABLE";',opts)
   self.assertNotIn('android.permission.INTERNET',(root/'app/src/main/AndroidManifest.xml').read_text())
 def test_bad_title_anchor_is_atomic(self):
  with tempfile.TemporaryDirectory() as d:
   root=pathlib.Path(d);self.fixture(root)
   p=root/'src/game/exit_menu.cpp';p.write_text(p.read_text().replace('bool JoinRing(', 'bool UnknownRing('))
   before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
   with self.assertRaises(ValueError):prepare(root)
   self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
 def test_repeat_prepare_does_not_overwrite(self):
  with tempfile.TemporaryDirectory() as d:
   root=pathlib.Path(d);self.fixture(root);prepare(root)
   before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
   with self.assertRaises(ValueError):prepare(root)
   self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
