import unittest,hashlib
from original_host_actor_callsites_xbe import (
 audit,OWNER_SHA1,MAX_FILE,TARGETS,
 direct_target,summarize_predecessors,UnsafeImage)
class HostActorCallsiteAuditTest(unittest.TestCase):
 def test_strict_direct_calls(self):
  self.assertEqual(direct_target("call","0x001e0e40"),0x001e0e40)
  self.assertEqual(direct_target("call","001E0E40 <some_function>"),0x001e0e40)
  self.assertIsNone(direct_target("call","[eax+4]"))
  self.assertIsNone(direct_target("jmp","0x001e0e40"))
  self.assertIsNone(direct_target("call","*eax"))
  self.assertIsNone(direct_target("call","garbage"))
 def test_preceding_register_classification_only(self):
  s=summarize_predecessors([
    ("mov","edx,ecx"),("mov","eax,0"),("push","ebx"),
    ("lea","esi,[edi+1]"),("push","eax"),("call","0x00200000")])
  self.assertEqual(s["nearby_push_count"],2)
  self.assertEqual(s["registers_written"],["eax","edx","esi"])
  self.assertEqual(s["window_size"],6)
 def test_owner_pin_gate(self):
  self.assertEqual(len(TARGETS),5)
  self.assertEqual(len(set(TARGETS.values())),5)
  self.assertEqual(len(OWNER_SHA1),40)
  for data in (b"",b"fake",b"XBEH"+bytes(120),b"XBEH"+bytes(MAX_FILE)):
   with self.assertRaises(UnsafeImage):audit(data)
  with self.assertRaises(UnsafeImage):audit(b"fake","bad")
  self.assertNotEqual(hashlib.sha1(b"fake").hexdigest(),OWNER_SHA1)
if __name__=="__main__":unittest.main()
