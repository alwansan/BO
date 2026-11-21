"""Pinned c5 factory original ABI guard, not permission to create a human actor."""
import re
def verify_factory_body(body):
 if 'x86_trap(' in body:raise ValueError('Trapping factory implementation')
 def block(address):
  matches=re.findall(r'// '+address+r' [^\n]*\n(.*?)(?=\s*// [0-9A-F]{8} |\Z)',body,re.S)
  if len(matches)!=1:raise ValueError('Missing/duplicate factory instruction '+address)
  return re.sub(r'\s+','',matches[0])
 for address in ('001DF467','001DF590'):
  if 'ctx.esp+=12;return;' not in block(address):raise ValueError('Wrong factory ret8 '+address)
 if 'sub_001D21F0(ctx,base);' not in block('001DF544'):raise ValueError('Missing independent controller initialization')
 if 'ctx.eax=(uint32_t)(ctx.ebp);' not in block('001DF588'):raise ValueError('Wrong actor return register')
