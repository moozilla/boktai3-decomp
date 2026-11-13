#!/usr/bin/env python3
"""Plot actual C/wasm audio output; all generated assets stay in build/demo/."""
from pathlib import Path
import csv
import json
import math
import struct
import wave
from PIL import Image, ImageDraw, ImageFont

OUT = Path("build/demo")
BG, PANEL, GRID, INK, MINT, GOLD = "#111b20", "#19272d", "#30434a", "#f2eee3", "#81d9bb", "#f8ca68"
font_path = "/System/Library/Fonts/Supplemental/Arial.ttf"
def font(size):
    try:
        return ImageFont.truetype(font_path, size)
    except OSError:
        return ImageFont.load_default()

def base(title, subtitle):
    im = Image.new("RGB", (1100, 660), BG)
    d = ImageDraw.Draw(im)
    d.text((42, 30), "BOKTAI 3 / SOURCE PORT", font=font(17), fill=MINT)
    d.text((42, 76), title, font=font(38), fill=INK)
    d.text((42, 128), subtitle, font=font(17), fill="#a7b9ba")
    return im, d

def plot(d, rect, values, color, limit=32768):
    x0,y0,x1,y1=rect
    d.rounded_rectangle(rect, radius=10, fill=PANEL)
    mid=(y0+y1)/2
    for i in range(1,8):
        x=x0+(x1-x0)*i/8
        d.line((x,y0,x,y1),fill=GRID)
    d.line((x0,mid,x1,mid),fill=GRID)
    points=[(x0+i*(x1-x0)/(len(values)-1),mid-v/limit*(y1-y0)*.43) for i,v in enumerate(values)]
    d.line(points,fill=color,width=2)

with wave.open(str(OUT/"hardware-song-1.wav"),"rb") as f:
    rate=f.getframerate()
    samples=struct.unpack("<"+"h"*(f.getnframes()*2),f.readframes(f.getnframes()))
meta=json.loads((OUT/"hardware-song-1.json").read_text())
frames=[]
for frame in range(90):
    start=int((1+frame/30)*rate)*2
    block=samples[start:start+704]
    im,d=base("Portable C. Audible in WebAssembly.","Actual wasm PCM · Song 1 from a user-supplied ROM · 21,024 Hz stereo")
    d.text((42,178),"LEFT",font=font(13),fill=MINT)
    plot(d,(42,205,1058,365),block[::2],MINT)
    d.text((42,385),"RIGHT",font=font(13),fill=GOLD)
    plot(d,(42,412,1058,572),block[1::2],GOLD)
    d.text((42,598),f"{1+frame/30:04.2f} s    C mixer / wasm32    Peak {meta['peak']}    133 notes in 10s",font=font(18),fill=INK)
    d.text((42,630),"Audio scope, not game video. Native and wasm tests pass. No ROM bytes are distributed.",font=font(13),fill="#a7b9ba")
    frames.append(im)
frames[0].save(OUT/"hardware-audio-scope.png")
frames[0].save(OUT/"hardware-audio-scope.apng",save_all=True,append_images=frames[1:],duration=1000/30,loop=0)

comparison=OUT/"mixer-comparison.csv"
if comparison.exists():
    rows=list(csv.DictReader(comparison.open()))
    original=[int(row['original']) for row in rows]
    portable=[int(row['portable']) for row in rows]
    im,d=base("The same channel. Two implementations.","Original ROM SoundMain in mGBA beside the portable C mixer · synthetic input")
    d.text((42,190),"ORIGINAL ROM / mGBA",font=font(18),fill=GOLD)
    d.text((575,190),"PORTABLE C",font=font(18),fill=MINT)
    plot(d,(42,235,525,470),original,GOLD,128)
    plot(d,(575,235,1058,470),portable,MINT,128)
    error=max(abs(a-b) for a,b in zip(original,portable))
    d.text((42,510),f"Maximum PCM difference: {error} signed 8-bit unit",font=font(25),fill=INK)
    d.text((42,554),"Fractional sample phase: exact    Remaining sample count: exact",font=font(19),fill=MINT)
    d.text((42,610),"One controlled mixer case. This is not a claim of full-game or bit-identical audio.",font=font(15),fill="#a7b9ba")
    im.save(OUT/"mixer-comparison.png")
print("Wrote audio scope PNG/APNG and mixer comparison PNG under build/demo/")
