import os
import pathlib
import shutil
import tempfile
import unittest
from prepare_probe import BASE_FILES, GAME_FILES
from prepare_room_network import prepare, PACKAGE, INPUTS

class RoomNetPrepareTests(unittest.TestCase):
    def source(self, root):
        upstream=pathlib.Path(os.environ['BO_UPSTREAM_REFERENCE'])
        for name in INPUTS:
            destination=root/name
            destination.parent.mkdir(parents=True,exist_ok=True)
            shutil.copy2(upstream/name,destination)
    def test_network_test_package_has_real_but_isolated_android_bridge(self):
        with tempfile.TemporaryDirectory() as directory:
            root=pathlib.Path(directory)
            self.source(root)
            modified=prepare(root)
            self.assertGreaterEqual(len(modified),26)
            self.assertEqual(PACKAGE,'org.bocoop.black.roomnet')
            manifest=(root/'app/src/main/AndroidManifest.xml').read_text()
            self.assertIn('android.permission.INTERNET',manifest)
            self.assertIn('android.permission.ACCESS_NETWORK_STATE',manifest)
            self.assertIn('android:exported="false"',manifest)
            self.assertIn('com.racas.black.coop.RoomLobbyActivity',manifest)
            self.assertNotIn('android.permission.MANAGE_EXTERNAL_STORAGE',manifest)
            self.assertIn('applicationId "'+PACKAGE+'"',(root/'app/build.gradle').read_text())
            java=(root/'app/src/main/java/com/racas/black/BlackActivity.java').read_text()
            self.assertIn('public void openCoopRoom(int row)',java)
            self.assertIn('RoomAppRooms.statusLine(row)',java)
            self.assertIn('RoomAppRooms.shutdown()',java)
            activity=(root/'app/src/main/java/com/racas/black/coop/RoomLobbyActivity.java').read_text()
            self.assertIn('RoomAppRooms.obtain()',activity)
            self.assertNotIn('if(session!=null)session.stop();',activity)
            self.assertIn('PINNED', (root/'app/src/main/java/com/racas/black/coop/RoomAppRooms.java').read_text())
            ui=(root/'src/game/options_menu.cpp').read_text()
            self.assertIn('blackcoop::OpenAndroidLobby(row->id)',ui)
            self.assertIn('blackcoop::AndroidRoomRow(-104)',ui)
            self.assertIn('g_room_route.Take(blackcoop::Page::Options)',ui)
            self.assertNotIn('case -102: return "NOT AVAILABLE";',ui)
            header=(root/'src/coop/room_net_ui.hpp').read_text()
            self.assertIn('std::chrono::milliseconds(500)',header)
            self.assertIn('SDL_GetAndroidJNIEnv()',header)
    def test_fail_closed_on_changed_anchor(self):
        with tempfile.TemporaryDirectory() as directory:
            root=pathlib.Path(directory)
            self.source(root)
            p=root/'src/game/options_menu.cpp'
            p.write_text(p.read_text().replace('    const Row* row = g_page.rows[g_page.top + slot];',
                                                    '    const Row* row = g_page.rows.at(g_page.top + slot);'))
            # Base probe read-only transform must reject this upstream drift.
            before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
            with self.assertRaises(ValueError):prepare(root)
            self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
    def test_no_overwrite_on_second_transform(self):
        with tempfile.TemporaryDirectory() as directory:
            root=pathlib.Path(directory)
            self.source(root)
            prepare(root)
            before={str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()}
            with self.assertRaises(ValueError):prepare(root)
            self.assertEqual(before,{str(p.relative_to(root)):p.read_bytes() for p in root.rglob('*') if p.is_file()})
