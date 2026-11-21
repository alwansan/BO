# Original read-only analysis helper. Set BLACK_OWNER_XBE to owner image path.
import pathlib,struct,hashlib,tempfile,subprocess,sys,os
p=pathlib.Path(os.environ.get('BLACK_OWNER_XBE','/home/a/Desktop/black/biso/Black_Xbox_Extracted/default.xbe'));b=p.read_bytes();assert hashlib.sha1(b).hexdigest()=='c5bfed25449c8212d7a976f15246e87ead57713f'
u=lambda o:struct.unpack_from('<I',b,o)[0];base=u(0x104);sections=[]
for i in range(u(0x11c)):
 flags,va,vs,raw,n,name=struct.unpack_from('<6I',b,u(0x120)-base+i*56);ni=name-base
 if flags&4:sections.append((b[ni:b.index(0,ni)].decode(),va,b[raw:raw+n]))
names=b'\0';offsets=[]
for name,_,_ in sections:offsets.append(len(names));names+=name.encode()+b'\0'
shi=len(names);names+=b'.shstrtab\0';out=bytearray(256);heads=[(0,)*10]
for (_,va,data),ni in zip(sections,offsets):
 off=len(out);out.extend(data);heads.append((ni,1,6,va,off,len(data),0,0,16,0))
ns=len(out);out.extend(names);heads.append((shi,3,0,0,ns,len(names),0,0,1,0))
while len(out)%4:out.append(0)
shoff=len(out)
for h in heads:out.extend(struct.pack('<10I',*h))
struct.pack_into('<16sHHIIIIIHHHHHH',out,0,b'\x7fELF\x01\x01\x01'+b'\0'*9,2,3,1,0x25a3f,0,shoff,0,52,0,0,40,len(heads),len(heads)-1)
with tempfile.TemporaryDirectory(prefix='black-private-actor-') as d:
 elf=pathlib.Path(d)/'code.elf';elf.write_bytes(out)
 if 'scan' in sys.argv:
  from collections import deque
  proc=subprocess.Popen(['llvm-objdump','-d','--no-show-raw-insn',str(elf)],stdout=subprocess.PIPE,text=True)
  history=deque(maxlen=5);remaining=0
  for line in proc.stdout:
   if '0x2d1980' in line.lower():
    print('\nROOT REFERENCE');print(''.join(history));remaining=12
   if remaining:print(line,end='');remaining-=1
   history.append(line)
  proc.wait();raise SystemExit(proc.returncode)
 for spec in sys.argv[1:]:
  lo,hi=[int(s,16) for s in spec.split(':')]
  r=subprocess.run(['llvm-objdump','-d','--no-show-raw-insn',f'--start-address={lo}',f'--stop-address={hi}',str(elf)],capture_output=True,text=True,check=True)
  print(r.stdout)
