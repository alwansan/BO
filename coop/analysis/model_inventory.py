"""Read-only, bounded MODEL dictionary candidate inventory. No meshes or assets exported.
Layout reference: portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628
 docs/file-formats/{bin-db,models}.md (MIT). Original BO implementation.
This scans dictionary signatures and validates metadata; it is not a complete container parser.
"""
import argparse,json,pathlib,struct,math
MAGICS=(0x95820AF2485C0000,0x96F6A8E27244EA15)
MODEL=0x95820AD4B8000000
ALPHABET=' -/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_'
def span(data,offset,size):
 if offset<0 or size<0 or offset>len(data) or size>len(data)-offset:raise ValueError('Out of file')
 return memoryview(data)[offset:offset+size]
def u32(data,offset):return struct.unpack('<I',span(data,offset,4))[0]
def u64(data,offset):return struct.unpack('<Q',span(data,offset,8))[0]
def label(value):
 result=''
 for _ in range(12):value,digit=divmod(value,40);result=ALPHABET[digit]+result
 return result.rstrip() if value==0 else None

def model(data,base):
 span(data,base,0x70)
 if u64(data,base+0x10)!=MODEL or u32(data,base+0x18)!=0x23:raise ValueError('Unsupported model')
 count=u32(data,base+0x5C);buffers=u32(data,base+0x24)
 if count>512 or buffers>4096:raise ValueError('Model metadata limit')
 names_base=base+u32(data,base+0x60);span(data,names_base,count*4)
 span(data,base+u32(data,base+0x54),count*64)
 span(data,base+u32(data,base+0x58),count*4)
 span(data,base+u32(data,base+0x1C),buffers*0x1C)
 tags=set()
 for i in range(count):
  name_at=base+u32(data,names_base+i*4);raw=bytes(span(data,name_at,min(128,len(data)-name_at)))
  end=raw.find(b'\0')
  if end<0:raise ValueError('Unterminated bone name')
  name=raw[:end].decode('ascii').lower()
  for tag,words in [('head',('head','neck')),('torso',('spine','pelvis','hip')),('legs',('thigh','calf','knee','foot','ankle')),('arms',('arm','hand','finger'))]:
   if any(word in name for word in words):tags.add(tag)
 matrices=span(data,base+u32(data,base+0x54),count*64)
 if any(not math.isfinite(v[0]) for v in struct.iter_unpack('<f',matrices)):raise ValueError('Nonfinite matrix')
 return {'model_version':35,'bone_names':count,'buffers':buffers,'bone_tags':sorted(tags),'full_body_candidate':{'head','torso','legs'}<=tags}

def inspect(data):
 """Validated signature candidates only; never infer resource length from next offset."""
 result=[];seen=set()
 for magic in MAGICS:
  needle=struct.pack('<Q',magic);cursor=0
  while True:
   base=data.find(needle,cursor)
   if base<0:break
   cursor=base+8
   try:
    count=u32(data,base+8)
    if count>16384:continue
    table=base+u32(data,base+12);span(data,table,count*16)
   except (ValueError,struct.error):continue
   for i in range(count):
    try:
     entry=table+i*16;key=u64(data,entry);target=base+u32(data,entry+8)
     if (target,key) in seen:continue
     item=model(data,target);seen.add((target,key));item.update(resource_key=label(key),resource_key_hex=f'{key:016x}');result.append(item)
    except (ValueError,struct.error,UnicodeError):continue
 return result

def main():
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('root',type=pathlib.Path);parser.add_argument('--output',type=pathlib.Path,required=True)
 args=parser.parse_args();root=args.root
 if root.is_symlink():parser.error('Symlink input refused')
 files=[root] if root.is_file() else sorted(p for p in root.rglob('*') if p.is_file() and not p.is_symlink() and p.suffix.lower() in ('.bin','.db'))
 summary={'scope':'validated MODEL dictionary signature candidates; not complete container or renderer validation','files_examined':0,'files_skipped_size':0,'models':[]}
 for path in files:
  if path.stat().st_size>128*1024*1024:summary['files_skipped_size']+=1;continue
  summary['files_examined']+=1
  for item in inspect(path.read_bytes()):item['file']=str(path.relative_to(root)) if root.is_dir() else path.name;summary['models'].append(item)
 summary['model_candidates']=len(summary['models']);summary['full_body_candidates']=sum(x['full_body_candidate'] for x in summary['models'])
 args.output.write_text(json.dumps(summary,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
 print(json.dumps({k:v for k,v in summary.items() if k!='models'}))
if __name__=='__main__':main()
