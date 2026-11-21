import os
import pathlib
import shutil
import tempfile
import unittest
import xml.etree.ElementTree as ET

from prepare_probe import BASE_FILES, GAME_FILES, PROBE_FILES, PACKAGE, prepare

class ProbePrepareTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        value=os.environ.get("BO_UPSTREAM_REFERENCE")
        if not value:
            raise RuntimeError("BO_UPSTREAM_REFERENCE must be a pinned v1.5.0 checkout")
        cls.upstream=pathlib.Path(value).resolve()
        cls.probes=pathlib.Path(__file__).parent/"probe"

    def fixture(self,where):
        for name in BASE_FILES+GAME_FILES:
            dest=where/name
            dest.parent.mkdir(parents=True,exist_ok=True)
            shutil.copy2(self.upstream/name,dest)
        before={p:p.read_bytes() for p in where.rglob("*") if p.is_file()}
        return before

    def test_full_source_preparer_only_allowlisted_deltas(self):
        with tempfile.TemporaryDirectory() as d:
            dest=pathlib.Path(d)
            old=self.fixture(dest)
            changed=prepare(dest,self.probes)
            self.assertEqual(set(changed), set(BASE_FILES)|set(GAME_FILES)|
                             {f"src/coop/{n}" for n in PROBE_FILES})
            self.assertEqual(len(changed),10)
            self.assertIn(f'applicationId "{PACKAGE}"',(dest/"app/build.gradle").read_text())
            self.assertIn("READ ONLY",(dest/"app/src/main/res/values/strings.xml").read_text())
            java=(dest/"app/src/main/java/com/racas/black/BlackActivity.java").read_text()
            self.assertIn("private static native void nativeCoopProbeEnable();",java)
            self.assertIn("Play without capture",java)
            self.assertIn("if (ok) offerCoopProbe();",java)
            self.assertIn('new File(getExternalFilesDir(null), "assets");',java)
            self.assertIn("bo_probe::ObservePlayer(me,base);",(dest/"src/game/cheats.cpp").read_text())
            self.assertIn("bo_probe::ObserveAnimation(ctx.esp,base);",(dest/"src/game/hooks.cpp").read_text())
            self.assertEqual((dest/"src/game/hooks.cpp").read_text().count("X86_FUNC(sub_0017B130)"),1)
            self.assertEqual((dest/"src/game/cheats.cpp").read_text().count("__imp__sub_001E5FA0(ctx, base)"),1)
            m=ET.parse(dest/"app/src/main/AndroidManifest.xml")
            permissions={e.get('{http://schemas.android.com/apk/res/android}name') for e in m.findall("uses-permission")}
            self.assertFalse(any("EXTERNAL_STORAGE" in (p or "") for p in permissions))
            self.assertEqual(m.find(".//activity").get('{http://schemas.android.com/apk/res/android}name'),
                            "com.racas.black.BlackActivity")
            self.assertTrue(old.keys() <= {p for p in dest.rglob("*") if p.is_file()})

    def test_unknown_anchor_fails_before_any_write(self):
        with tempfile.TemporaryDirectory() as d:
            dest=pathlib.Path(d)
            self.fixture(dest)
            hooks=dest/"src/game/hooks.cpp"
            hooks.write_text(hooks.read_text().replace("X86_FUNC(sub_0017B130) {",
                                                      "X86_FUNC(incompatible_hook) {",1))
            before={p.relative_to(dest):p.read_bytes() for p in dest.rglob("*") if p.is_file()}
            with self.assertRaisesRegex(ValueError,"anchor"):
                prepare(dest,self.probes)
            after={p.relative_to(dest):p.read_bytes() for p in dest.rglob("*") if p.is_file()}
            self.assertEqual(before,after)

    def test_second_run_rejected_without_corruption(self):
        with tempfile.TemporaryDirectory() as d:
            dest=pathlib.Path(d)
            self.fixture(dest)
            prepare(dest,self.probes)
            before={p.relative_to(dest):p.read_bytes() for p in dest.rglob("*") if p.is_file()}
            with self.assertRaises(ValueError):
                prepare(dest,self.probes)
            self.assertEqual(before,{p.relative_to(dest):p.read_bytes() for p in dest.rglob("*") if p.is_file()})
if __name__=="__main__":
    unittest.main()
