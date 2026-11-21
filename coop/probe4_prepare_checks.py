import os,pathlib,shutil,tempfile,unittest,xml.etree.ElementTree as ET
from prepare_probe import BASE_FILES,GAME_FILES
from prepare_probe4 import prepare,PACKAGE
class Probe4PrepareTests(unittest.TestCase):
    def fixture(self,root):
        source=pathlib.Path(os.environ['BO_UPSTREAM_REFERENCE'])
        for name in BASE_FILES+GAME_FILES:
            p=root/name;p.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source/name,p)
    def test_separate_package_and_pool_sources(self):
        with tempfile.TemporaryDirectory() as d:
            root=pathlib.Path(d);self.fixture(root)
            changes=prepare(root)
            self.assertEqual(len(changes),15)
            self.assertIn('applicationId "org.bocoop.black.probe4"',(root/'app/build.gradle').read_text())
            self.assertIn('BLACK Probe 4 - READ ONLY',(root/'app/src/main/res/values/strings.xml').read_text())
            self.assertEqual({p.name for p in (root/'src/coop').iterdir()},
                             {'probe.cpp','probe.h','probe_core.hpp','pool_snapshot.hpp','pool_observer.inc','instance_graph.hpp'})
            java=(root/'app/src/main/java/com/racas/black/BlackActivity.java').read_text()
            self.assertIn('BLACK Probe 4',java);self.assertIn('probeLogExporter.start();',java)
            m=ET.parse(root/'app/src/main/AndroidManifest.xml')
            permissions={e.get('{http://schemas.android.com/apk/res/android}name') for e in m.findall('uses-permission')}
            self.assertNotIn('android.permission.INTERNET',permissions)
            self.assertEqual(PACKAGE,'org.bocoop.black.probe4')
    def test_bad_source_anchor_leaves_target_unchanged(self):
        with tempfile.TemporaryDirectory() as d:
            root=pathlib.Path(d);self.fixture(root)
            p=root/'src/game/cheats.cpp';p.write_text(p.read_text().replace('X86_FUNC(sub_001E5FA0)', 'X86_FUNC(unknown)'))
            before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
            with self.assertRaises(ValueError):prepare(root)
            self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
    def test_existing_extra_file_and_second_run_refused_before_writes(self):
        with tempfile.TemporaryDirectory() as d:
            root=pathlib.Path(d);self.fixture(root);prepare(root)
            before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
            with self.assertRaises(ValueError):prepare(root)
            self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
if __name__=='__main__':unittest.main()
