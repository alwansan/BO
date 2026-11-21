"""Fail closed on pinned generated reset operations; static shape, not runtime proof."""
import re

def verify_body(body):
    if 'x86_trap(' in body:raise ValueError('Trapping reset implementation')
    def block(address):
        matches=re.findall(r'// '+address+r' [^\n]*\n(.*?)(?=\s*// [0-9A-F]{8} |\Z)',body,re.S)
        if len(matches)!=1:raise ValueError('Missing/duplicate reset instruction '+address)
        return re.sub(r'\s+','',matches[0])
    if 'ctx.eax=(uint32_t)(0x1u);' not in block('001D5F08'):
        raise ValueError('Successful EAX1 assignment missing')
    store=block('001D5F0E')
    if not all(token in store for token in ('uint32_tea=(uint32_t)(ctx.edi+0x8524u);','X86_MEM32(ea)=(uint32_t)(ctx.eax);')):
        raise ValueError('Successful counter store missing or wrong destination/value')
    for address in ('001D5E1B','001D5E2D','001D5F29'):
        if 'ctx.esp+=4;return;' not in block(address):raise ValueError('Wrong reset return contract '+address)
    failed=block('001D5E19')
    if not all(token in failed for token in ('uint8_ta=(uint8_t)(((uint8_t)ctx.eax));','uint8_tb=(uint8_t)(((uint8_t)ctx.eax));','a^b','ctx.eax=(ctx.eax&0xFFFFFF00u)|(uint8_t)(r);')):
        raise ValueError('Failed reset must zero AL preserving upper EAX')
