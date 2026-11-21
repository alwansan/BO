import pathlib
import tempfile
import unittest
import xml.etree.ElementTree as ET
from prepare_baseline import prepare

class BaselinePrepareTest(unittest.TestCase):
    def fixture(self, root):
        files = {
            'app/build.gradle': 'namespace "com.racas.black"\napplicationId "com.racas.black"\narguments "-DANDROID_STL=c++_shared"\n',
            'app/src/main/AndroidManifest.xml': '<manifest xmlns:android="http://schemas.android.com/apk/res/android"><uses-permission android:name="android.permission.MANAGE_EXTERNAL_STORAGE" /><uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" android:maxSdkVersion="32" /><uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" android:maxSdkVersion="29" /><application><activity android:name=".BlackActivity"/></application></manifest>',
            'app/src/main/res/values/strings.xml': '<resources><string name="app_name">Black Android</string></resources>',
            'app/src/main/java/com/racas/black/UpdateChecker.java': 'class UpdateChecker {\nstatic void check(Context context, boolean manual, Listener listener) {\noriginalCheck();\n}\n}\n',
            'app/src/main/java/com/racas/black/BlackActivity.java': 'private void askAllFilesAccess() { }\nFile dir = hasStorageAccess()\n                ? new File(Environment.getExternalStorageDirectory(), SHARED)\n                : new File(getExternalFilesDir(null), "assets");',
            'native/game_files.cpp': 'original runtime hash verification',
            'src/game/hooks.cpp': 'original gameplay hooks',
        }
        for name, content in files.items():
            p=root/name
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text(content)
        return files

    def test_isolates_install_and_preserves_native_gameplay(self):
        with tempfile.TemporaryDirectory() as d:
            root=pathlib.Path(d)
            original=self.fixture(root)
            changed=prepare(root)
            gradle=(root/'app/build.gradle').read_text()
            self.assertIn('applicationId "org.bocoop.black.baseline"', gradle)
            self.assertIn('namespace "com.racas.black"', gradle)
            manifest=ET.parse(root/'app/src/main/AndroidManifest.xml')
            self.assertEqual(manifest.find('.//activity').get('{http://schemas.android.com/apk/res/android}name'), 'com.racas.black.BlackActivity')
            permissions=[e.get('{http://schemas.android.com/apk/res/android}name') for e in manifest.findall('uses-permission')]
            self.assertFalse(any('EXTERNAL_STORAGE' in p for p in permissions))
            labels=ET.parse(root/'app/src/main/res/values/strings.xml')
            self.assertIn('NO CO-OP', labels.find('.//string').text)
            self.assertEqual(set(changed), {p for p in original if p.startswith('app/')})
            for p in ('native/game_files.cpp','src/game/hooks.cpp'):
                self.assertEqual((root/p).read_text(), original[p])

    def test_unknown_source_fails_before_any_write(self):
        with tempfile.TemporaryDirectory() as d:
            root=pathlib.Path(d)
            original=self.fixture(root)
            updater=root/'app/src/main/java/com/racas/black/UpdateChecker.java'
            updater.write_text('unknown upstream revision')
            before={p:p.read_bytes() for p in root.rglob('*') if p.is_file()}
            with self.assertRaises(ValueError):
                prepare(root)
            self.assertEqual(before, {p:p.read_bytes() for p in root.rglob('*') if p.is_file()})

if __name__=='__main__': unittest.main()
