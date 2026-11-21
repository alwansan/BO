import unittest,struct
from model_inventory import inspect,MAGICS,MODEL
class InventoryTests(unittest.TestCase):
 def fixture(self):
  b=bytearray(0x400)
  struct.pack_into('<QII',b,0,MAGICS[0],1,0x20);struct.pack_into('<QII',b,0x20,123,0x40,0)
  struct.pack_into('<QI',b,0x50,MODEL,35)
  for off,value in [(0x1C,0x70),(0x24,0),(0x54,0x80),(0x58,0x180),(0x5C,3),(0x60,0x190)]:struct.pack_into('<I',b,0x40+off,value)
  for i,name in enumerate([b'Head\0',b'Spine\0',b'Foot\0']):
   struct.pack_into('<I',b,0x1D0+i*4,0x200+i*16);b[0x240+i*16:0x240+i*16+len(name)]=name
  return b
 def test_body_is_candidate_and_offsets_use_model_base(self):
  r=inspect(self.fixture());self.assertEqual(len(r),1);self.assertTrue(r[0]['full_body_candidate']);self.assertEqual(r[0]['bone_names'],3)
 def test_truncated_and_overflow_candidates_refused(self):
  for n in range(0,0x60):self.assertEqual(inspect(self.fixture()[:n]),[])
  b=self.fixture();struct.pack_into('<I',b,12,0xFFFFFFFF);self.assertEqual(inspect(b),[])
  b=self.fixture();struct.pack_into('<I',b,0x9C,513);self.assertEqual(inspect(b),[])
 def test_no_leg_no_full_body_claim(self):
  b=self.fixture();b[0x260:0x265]=b'Hand\0';self.assertFalse(inspect(b)[0]['full_body_candidate'])
