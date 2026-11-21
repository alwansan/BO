import unittest
from verify_probe3_reset import verify_body
# Handwritten minimal emitter-format fixture, not game source or bytes.
VALID='''
// 001D5E19 xor al, al
{ uint8_t a = ((uint8_t)ctx.eax); uint8_t b = ((uint8_t)ctx.eax); uint8_t r = (uint8_t)(a ^ b); ctx.eax = (ctx.eax & 0xFFFFFF00u) | (uint8_t)(r); }
// 001D5E1B ret
ctx.esp += 4; return;
// 001D5E2D ret
ctx.esp += 4; return;
// 001D5F08 mov eax, 1
ctx.eax = (uint32_t)(0x1u);
// 001D5F0E mov [edi+8524], eax
X86_MEM32(ctx.edi + 0x8524u) = (uint32_t)(ctx.eax);
// 001D5F29 ret
ctx.esp += 4; return;
'''
class ResetGuardTests(unittest.TestCase):
 def test_matching_operations_accepted(self):verify_body(VALID)
 def test_wrong_stack_contract_rejected(self):
  with self.assertRaises(ValueError):verify_body(VALID.replace('ctx.esp += 4','ctx.esp += 12',1))
 def test_comment_or_constant_without_actual_store_rejected(self):
  with self.assertRaises(ValueError):verify_body(VALID.replace('X86_MEM32(ctx.edi + 0x8524u) = (uint32_t)(ctx.eax);','(void)0x8524u;'))
 def test_wrong_success_or_failed_return_rejected(self):
  for bad in [VALID.replace('(0x1u)','(0x0u)'),VALID.replace('a ^ b','a | b'),VALID.replace('0xFFFFFF00u','0xFFFF0000u')]:
   with self.subTest(bad=bad),self.assertRaises(ValueError):verify_body(bad)
if __name__=='__main__':unittest.main()
