import unittest
from verify_factory_hook import verify_factory_body
GOOD='''// 001DF467 ret 8
ctx.esp += 12; return;
// 001DF544 call 1D21F0
sub_001D21F0(ctx,base);
// 001DF588 mov eax,ebp
ctx.eax = (uint32_t)(ctx.ebp);
// 001DF590 ret 8
ctx.esp += 12; return;
'''
class FactoryGuardTests(unittest.TestCase):
 def test_pinned_contract(self):verify_factory_body(GOOD)
 def test_drift_rejected(self):
  for bad in [GOOD.replace('+= 12','+= 4'),GOOD.replace('sub_001D21F0','sub_001D21F1'),GOOD.replace('ctx.ebp','ctx.edi'),GOOD+'x86_trap(ctx,0,0);',GOOD+GOOD]:
   with self.assertRaises(ValueError):verify_factory_body(bad)
