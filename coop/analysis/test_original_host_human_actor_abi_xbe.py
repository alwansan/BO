import hashlib,unittest
from original_host_human_actor_abi_xbe import audit,CANDIDATES,WINDOW,OWNER_SHA1,UnsafeImage,MAX_FILE

class OriginalActorCandidateTest(unittest.TestCase):
 def test_pin_and_candidate_set_fixed(self):
  self.assertEqual(WINDOW,320)
  self.assertEqual(len(CANDIDATES),8)
  self.assertEqual(len(set(CANDIDATES.values())),8)
  for key in ("host_physics_proxy_acquire","host_physics_proxy_release",
              "original_player_setup","owner_camera_initializer",
              "original_player_tick","soldier_anim_select"):
   self.assertIn(key,CANDIDATES)
  self.assertEqual(len(OWNER_SHA1),40)
 def test_wrong_owner_rejected(self):
  data=b"XBEH"+b"\0"*120
  self.assertNotEqual(hashlib.sha1(data).hexdigest(),OWNER_SHA1)
  for image in (b"",b"bad",data):
   with self.assertRaises(UnsafeImage):audit(image)
 def test_incorrect_pin_and_oversized_rejected(self):
  image=b"XBEH"+b"\0"*120
  with self.assertRaises(UnsafeImage):audit(image,"0"*40)
  with self.assertRaises(UnsafeImage):audit(image,"bad")
  with self.assertRaises(UnsafeImage):audit(b"A"*(MAX_FILE+1))

if __name__=="__main__":unittest.main()
