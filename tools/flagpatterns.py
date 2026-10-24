#!/usr/bin/env python3
"""Look for compiler-flag fingerprints in a ROM, independent of our own C.

  flagpatterns.py FORK_AGBCC FLAG FLAGSURVEY.json   (env: ROM=other.gba, KEY=narrow|shift)

For every unit that tools/flagsurvey.py reports as changed by FLAG (KEY selects the column),
compile it with the pinned agbcc and with FORK_AGBCC + FLAG, assemble both, and cut out the
code blocks that differ (BL pairs masked). Then count how often each form occurs anywhere
in the ROM (default baserom.gba; ROM=... for Boktai 1/2). A ROM built with the flagged
compiler contains the flag-side blocks and not the stock-side ones, independent of how we
wrote the C. Blocks of 7+ halfwords are the reliable ones; shorter ones match by chance.
Result for Boktai 1/2/3 and -fsigned-narrow-modes: docs/BOKTAI12_COMPARISON.md 6.1.
"""
import difflib
import json
import os
import re
import struct
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402

os.chdir(build.ROOT)
FORK, FLAG = sys.argv[1], sys.argv[2]
d = json.load(open(sys.argv[3]))
units=[k for k,v in d.items() if 'm' in v and not v['m'][os.environ.get('KEY','narrow')]]
def comp(c,flags,pre):
    with tempfile.TemporaryFile(mode='w+') as f:
        f.write(pre); f.seek(0)
        r=subprocess.run([c]+flags+['-o','-'],stdin=f,capture_output=True,text=True)
    return r.stdout if not r.returncode else None
def code(asm):
    with tempfile.TemporaryDirectory() as t:
        s=t+'/a.s'; open(s,'w').write(asm+"\t.text\n\t.align 2,0\n")
        subprocess.run(build.AS+['-o',t+'/a.o',s],check=True,capture_output=True)
        subprocess.run(['arm-none-eabi-objcopy','-O','binary','-j','.text',t+'/a.o',t+'/a.bin'],check=True)
        b=open(t+'/a.bin','rb').read()
    h=list(struct.unpack('<%dH'%(len(b)//2),b[:len(b)&~1]))
    for i in range(len(h)-1):
        if 0xF000<=h[i]<0xF800 and h[i+1]>=0xF800: h[i]=0xF000;h[i+1]=0xF800
    return h
rom=open(os.environ.get('ROM','baserom.gba'),'rb').read()[:0x1000000]
rh=list(struct.unpack('<%dH'%(len(rom)//2),rom))
for i in range(len(rh)-1):
    if 0xF000<=rh[i]<0xF800 and rh[i+1]>=0xF800: rh[i]=0xF000;rh[i+1]=0xF800
from collections import defaultdict
idx=defaultdict(list)
K=5
for i in range(len(rh)-K): idx[tuple(rh[i:i+K])].append(i)
def count(seq):
    if len(seq)<K: return None
    c=0
    for p in idx.get(tuple(seq[:K]),[]):
        if rh[p:p+len(seq)]==seq: c+=1
    return c
tot=[0,0]; rows=[]
for u in units:
    build.DEFINES[:]=[]
    try: pre=build.preprocess(u)
    except SystemExit: continue
    base=build.cflags_for(u)
    a=comp(build.AGBCC,base,pre); b=comp(FORK,base+[FLAG],pre)
    if not a or not b: continue
    try: ha,hb=code(a),code(b)
    except Exception as e: continue
    sm=difflib.SequenceMatcher(None,ha,hb,autojunk=False)
    for op,i1,i2,j1,j2 in sm.get_opcodes():
        if op=='equal': continue
        ca=ha[max(0,i1-2):i2+2]; cb=hb[max(0,j1-2):j2+2]
        na,nb=count(ca),count(cb)
        rows.append((u,len(ca),len(cb),na,nb))
        if na is not None: tot[0]+= na>0
        if nb is not None: tot[1]+= nb>0
for r in rows: print(*r)
print('blocks',len(rows),'stock-side found in ROM',tot[0],'narrow-side found in ROM',tot[1])
