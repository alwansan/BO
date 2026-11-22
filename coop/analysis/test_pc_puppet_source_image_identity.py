import hashlib,unittest
from verify_pc_puppet_source_image_identity import inspect,IdentityError,MAX_INPUT,OWNER_SHA1
class IdentityTest(unittest.TestCase):
    def test_unpinned_rejected(self):
        a=b"XBEH"+b"\0"*120
        with self.assertRaises(IdentityError):inspect(a)
    def test_bad_header_rejected(self):
        with self.assertRaises(IdentityError):inspect(b"BEHX"+b"\0"*120)
    def test_oversized_rejected(self):
        with self.assertRaises(IdentityError):inspect(b"XBEH"+b"\0"*(MAX_INPUT+1))
    def test_empty_rejected(self):
        with self.assertRaises(IdentityError):inspect(b"")
    def test_reference_pin_is_fixed(self):
        self.assertEqual(len(OWNER_SHA1),40)
        self.assertNotEqual(hashlib.sha1(b"fake").hexdigest(),OWNER_SHA1)
if __name__=="__main__":unittest.main()
