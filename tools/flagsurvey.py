#!/usr/bin/env python3
"""Simport sys,os,json,subprocess,glob,re,tempfile
urvey which translation units change when agbcc gets extra flags.

  flagsurvey.py FORK_AGBCC OUT.json

Compiles every src/*.c and src/fn/*.c (and the NONMATCHING branch where present) with the
pinned agbcc and with FORK_AGBCC under each flag set, and records whether the assembly
is identical to the pinned compiler's. A matched unit that changes would stop matching;
a NONMATCHING one that changes is a candidate to re-check with tools/check.py. Built for
akatsuki105/agbcc's -fsigned-narrow-modes and -ffix-shift-compare (the official 2003 AGB
"thumb patch"); edit CONFIGS for other flags. Result for Boktai 3: docs/BOKTAI12_COMPARISON.md.
"""
import glob
import json
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
from multiprocessing import Pool
FORK=sys.argv[1]; OUT=sys.argv[2]
STOCK=build.AGBCC
CONFIGS={"fork":[],"narrow":["-fsigned-narrow-modes"],"shift":["-ffix-shift-compare"],"both":["-fsigned-narrow-modes","-ffix-shift-compare"]}
def run(comp,flags,pre):
    with tempfile.TemporaryFile(mode="w+") as f:
        f.write(pre);f.seek(0)
        r=subprocess.run([comp]+flags+["-o","-"],stdin=f,cwd=build.ROOT,capture_output=True,text=True)
    return None if r.returncode or "error" in r.stderr else r.stdout
def work(src):
    res={}
    try: txt=open(os.path.join(build.ROOT,src),errors='ignore').read()
    except: return src,res
    if re.search(r"^// COMPILER:",txt,re.M): return src,{"skip":1}
    base=build.cflags_for(src)
    for name,defs in (("m",[]),("nm",["-DNONMATCHING"])):
        if name=="nm" and "NONMATCHING" not in txt: continue
        build.DEFINES[:]=defs
        try: pre=build.preprocess(src)
        except SystemExit: res[name]="prefail";continue
        stock=run(STOCK,base,pre)
        if stock is None: res[name]="stockfail";continue
        out={}
        for c,fl in CONFIGS.items():
            a=run(FORK,base+fl,pre)
            out[c]=(a==stock) if a is not None else "fail"
        res[name]=out
    return src,res
if __name__=="__main__":
    os.chdir(build.ROOT)
    files=sorted(glob.glob("src/*.c")+glob.glob("src/fn/*.c"))
    out={}
    with Pool(8) as p:
        for i,(s,r) in enumerate(p.imap_unordered(work,files,chunksize=8)):
            out[s]=r
            if i%500==0: print(i,len(files),flush=True)
    json.dump(out,open(OUT,"w"))
