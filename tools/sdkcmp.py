#!/usr/bin/env python3
"""Compare the pinned agbcc with another cc1 (e.g. the real AGB SDK compiler) on every unit.

  sdkcmp.py SDK_CC1 OUT.json [LIMIT]

Compiles each src/*.c and src/fn/*.c (and the NONMATCHING branch where present) with the
pinned agbcc and with SDK_CC1 (invoked as: cc1 -quiet FLAGS in.i -o out.s), assembles both
and compares the .text bytes. Values per unit and branch: same, diff, stockfail, sdkfail,
asmfail (usually a missing INCLUDE_ASM stub) or skip (units with a COMPILER override).
Build notes for the SDK cc1 and the result: docs/BOKTAI12_COMPARISON.md 6.2.
"""
import glob
import json
import os
import re
import subprocess
import sys
import tempfile
from collections import Counter
from multiprocessing import Pool

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402

SDK, OUT = sys.argv[1], sys.argv[2]
os.chdir(build.ROOT)
def asm_bytes(asm):
    with tempfile.TemporaryDirectory() as t:
        open(t+'/a.s','w').write(asm+"\t.text\n\t.align 2,0\n")
        r=subprocess.run(build.AS+['-o',t+'/a.o',t+'/a.s'],capture_output=True)
        if r.returncode: return None
        subprocess.run(['arm-none-eabi-objcopy','-O','binary','-j','.text',t+'/a.o',t+'/a.bin'],check=True)
        return open(t+'/a.bin','rb').read()
def work(src):
    txt=open(src,errors='ignore').read()
    if re.search(r"^// COMPILER:",txt,re.M): return src,'skip'
    res={}
    for name,defs in (("m",[]),("nm",["-DNONMATCHING"])):
        if name=="nm" and "NONMATCHING" not in txt: continue
        build.DEFINES[:]=defs
        try: pre=build.preprocess(src)
        except SystemExit: res[name]='prefail'; continue
        base=[f for f in build.cflags_for(src) if f!='-fhex-asm']
        outs=[]
        for comp,flags,isSdk in ((build.AGBCC,base+['-fhex-asm'],0),(SDK,['-quiet']+base,1)):
            with tempfile.TemporaryDirectory() as t:
                inp=t+'/in.i'; open(inp,'w').write(pre)
                if isSdk:
                    r=subprocess.run([comp]+flags+[inp,'-o',t+'/o.s'],capture_output=True,text=True)
                    a=open(t+'/o.s').read() if r.returncode==0 and os.path.exists(t+'/o.s') else None
                else:
                    r=subprocess.run([comp]+flags+['-o','-'],stdin=open(inp),capture_output=True,text=True)
                    a=r.stdout if r.returncode==0 and 'error' not in r.stderr else None
            outs.append(a)
        if outs[0] is None: res[name]='stockfail'; continue
        if outs[1] is None: res[name]='sdkfail'; continue
        outs=[re.sub(r'(?m)^(\s*\.align\s+\d+)\s*$',r'\1, 0',o) for o in outs]
        b0,b1=asm_bytes(outs[0]),asm_bytes(outs[1])
        if b0 is None or b1 is None: res[name]='asmfail'; continue
        res[name]='same' if b0==b1 else 'diff'
    return src,res
if __name__=='__main__':
    files=sorted(glob.glob('src/*.c')+glob.glob('src/fn/*.c'))
    if len(sys.argv)>3: files=files[:int(sys.argv[3])]
    out={}
    with Pool(8) as p:
        for i,(s,r) in enumerate(p.imap_unordered(work,files,chunksize=4)):
            out[s]=r
            if i%500==0: print(i,len(files),flush=True)
    json.dump(out,open(OUT,'w'))
    c=Counter(v.get('m') if isinstance(v,dict) else v for v in out.values()); print(c)
