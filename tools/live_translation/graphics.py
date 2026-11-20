"""Translate ink in CB05 sprite frames; preserve animation keys and unrelated art.

Each edited frame gets its own part list and 4bpp tile rectangles. This matters
for menu resources where a single Japanese glyph tile appears in several labels.
"""
from collections import Counter
from functools import lru_cache
from io import BytesIO
import json
from pathlib import Path
import struct
from PIL import Image, ImageDraw, ImageFont, ImageFilter
from runtime import HERE, OUT
import archive
import sprites

REGIONS = json.loads((HERE/"art.json").read_text())
SIZES = {size:(shape,i) for shape,sizes in sprites.SIZES.items() for i,size in enumerate(sizes)}
BASE = 0x08000000


@lru_cache(maxsize=24)
def font(size, heavy=False):
    faces = (["Impact.ttf", "DejaVuSansCondensed-Bold.ttf", "Arial Black.ttf"] if heavy else
             ["Arial Bold.ttf","DejaVuSans-Bold.ttf"])
    for face in faces:
        try:
            return ImageFont.truetype(face,size)
        except OSError:
            continue
    return ImageFont.load_default(size=size)


def letter_mask(text, width, height, heavy=False):
    if not text.strip() or width < 1 or height < 1:
        raise ValueError("Empty graphic or empty lettering")
    face = font(max(24,height*3),heavy)
    box = face.getbbox(text)
    mask = Image.new("L",(max(1,box[2]-box[0]),max(1,box[3]-box[1])))
    ImageDraw.Draw(mask).text((-box[0],-box[1]),text,255,font=face)
    target_w=min(width, round(mask.width*height/mask.height))
    return mask.resize((max(1,target_w),height),Image.Resampling.LANCZOS).point(lambda p: 255 if p>=112 else 0)


def gradient_text(image, mask, x, y, top, bottom):
    draw=ImageDraw.Draw(image)
    for dy in range(mask.height):
        weight=dy/max(1,mask.height-1)
        color=tuple(round(a+(b-a)*weight) for a,b in zip(top,bottom))
        for dx in range(mask.width):
            if mask.getpixel((dx,dy)): draw.point((x+dx,y+dy),fill=color+(255,))


def lettering(source, english, style):
    w,h=source.size
    out=Image.new("RGBA",(w,h))
    if style=="masthead":
        m=letter_mask(english.upper(),w-24, min(45,h-15),True)
        # Slant the face and kick out the lower edge, as on the cartridge logo.
        m=m.transform((m.width+10,m.height),Image.Transform.AFFINE,(1,.21,-9,0,1,0))
        silhouette=Image.new("L", (w,h))
        silhouette.paste(m,((w-m.width)//2,(h-m.height)//2))
        for offset in range(5,0,-1):
            out.paste((85,28,26,255),(0,offset),silhouette.filter(ImageFilter.MaxFilter(7)))
        out.paste((255,197,34,255),(0,0),silhouette.filter(ImageFilter.MaxFilter(5)))
        out.paste((104,39,25,255),(0,0),silhouette.filter(ImageFilter.MaxFilter(3)))
        x0,y0=(w-m.width)//2,(h-m.height)//2
        bands=[(255,255,244),(255,235,157),(239,159,32),(195,54,8),(247,107,10),(255,232,23)]
        blue=[(255,255,255),(197,237,255),(85,160,213),(23,69,130),(11,104,175),(165,224,249)]
        for y in range(m.height):
            t=y/max(1,m.height-1)*(len(bands)-1)
            index=min(int(t),len(bands)-2); mix=t-index
            for x in range(m.width):
                if m.getpixel((x,y)):
                    cols=blue if x < .20*m.width else bands
                    out.putpixel((x0+x,y0+y),tuple(round(a+(b-a)*mix) for a,b in zip(cols[index],cols[index+1]))+(255,))
        return out
    if style=="subtitle":
        # Its separate rule and glint are part of the design, not lettering.
        out.paste(source.crop((0,h-3,w,h)),(0,h-3))
        m=letter_mask(english.upper(),w-8,h-8)
        x,y=(w-m.width)//2,1
        out.paste((125,8,18,255),(x+1,y+1),m)
        gradient_text(out,m,x,y,(247,99,101),(191,7,26))
        return out
    if style=="buttons":
        words=[s.strip() for s in english.split("|")]
        slots=[(0,w)] if len(words)==1 else [(0,56),(56,w)]
        if len(words)!=len(slots):
            raise ValueError("Unexpected button group count")
        for word,(start,end) in zip(words,slots):
            out.paste(source.crop((start,0,start+8,h)),(start,0))
            m=letter_mask(word,end-start-9,7)
            out.paste((164,106,106,255),(start+9,0),m)
        return out
    m=letter_mask(english.upper(),w-2,min(11,h-2),True)
    x,y=(w-m.width)//2,(h-m.height)//2
    out.paste((190,119,79,255),(x+1,y+1),m)
    gradient_text(out,m,x,y,(255,246,156),(238,189,115))
    return out


def palette_pool(rom):
    entry=next(entries[0] for _,t,_,entries in archive.archives(rom) if t==0x9b1b)
    pos=entry[1]-BASE
    count=struct.unpack_from("<H",rom,pos)[0]
    values=struct.unpack_from(f"<{count*16}H",rom,pos+4)
    return [[tuple(((v>>k)&31)*255//31 for k in (0,5,10)) for v in values[i:i+16]]
            for i in range(0,len(values),16)]


def rectangles(image):
    # Break into legal OBJ sizes, bounded by the original bounding box.
    w,h=image.size
    y=0
    while y<h:
        rh=32 if h-y>=32 else 16 if h-y>=16 else 8
        x=0
        while x<w:
            widths=sorted((sw for sw,sh in SIZES if sh==rh),reverse=True)
            rw=next((sw for sw in widths if sw<=w-x),8)
            yield x,y,rw,rh
            x+=rw
        y+=rh


def quantize(im,palettes,banks):
    colors=Counter(rgb[:3] for rgb in im.getdata() if rgb[3])
    if not colors: return None
    best=None
    for bank in banks:
        mapping={}; score=0
        for color,n in colors.items():
            distance,idx=min((sum((x-y)**2 for x,y in zip(color,rgb)),i)
                             for i,rgb in enumerate(palettes[bank]) if i)
            score+=distance*n;mapping[color]=idx
        if best is None or score<best[0]:
            best=score,bank,mapping
    return best[1], [best[2].get(rgb[:3],0) if rgb[3] else 0 for rgb in im.getdata()]


class PackageEditor:
    def __init__(self, package, palettes):
        self.p=package
        self.palettes=palettes
        f,a,p,k,t=package.offs
        self.sections=[bytearray(package.data[x:y]) for x,y in zip((f,a,p,k,t),(a,p,k,t,len(package.data)))]
        self.frames=list(package.frames())
        self.changed=0

    def replace(self, index, image):
        frame=self.frames[index]
        parts=list(self.p.parts(frame))
        left,top=min(p["x"] for p in parts),min(p["y"] for p in parts)
        banks=sorted({p["palette"] for p in parts})
        part_data,tile_data=self.sections[2],self.sections[4]
        start=len(part_data)
        for x,y,w,h in rectangles(image):
            result=quantize(image.crop((x,y,x+w,y+h)),self.palettes,banks)
            if result is None: continue
            bank,indices=result
            first=len(tile_data)//32
            # Source tiles use a fixed 16-tile row pitch. No alias with existing frames.
            ntiles=(h//8-1)*16+w//8
            if first+ntiles>4096:
                raise ValueError("Translated sprite exceeds 12-bit tile indices")
            tile_data.extend(bytes(ntiles*32))
            for py in range(h):
                for px in range(0,w,2):
                    tile=first+(py//8)*16+px//8
                    off=tile*32+(py%8)*4+(px%8)//2
                    tile_data[off]=indices[py*w+px] | indices[py*w+px+1]<<4
            shape,size=SIZES[w,h]
            part_data.extend(struct.pack("<HhhH",shape<<8|size<<10,left+x,top+y,first|bank<<12))
        struct.pack_into("<H",self.sections[0],index*20+2,(len(part_data)-start)//8)
        struct.pack_into("<I",self.sections[0],index*20+16,start)
        self.changed+=1

    def pack(self):
        header=bytearray(self.p.data[:32])
        struct.pack_into("<H",header,6,len(self.sections[2])//8)
        struct.pack_into("<H",header,10,len(self.sections[4])//32)
        for i,section in enumerate(self.sections):
            struct.pack_into("<I",header,12+4*i,len(header))
            header.extend(section)
        return bytes(header)


class ArtTranslator:
    def __init__(self):
        self.loaded={}
        self.pool=None

    def __call__(self, resolver, game, key, ptr, kind, lr, scene, proc, frame):
        code=f"{key:04X}"
        if code not in REGIONS:
            return 0
        if key in self.loaded:
            return self.loaded[key]
        if self.pool is None:
            self.pool=palette_pool(game.rom)
        package=sprites.package(game.rom,key)
        palettes=self.pool[package.h0:package.h0+16]
        editor=PackageEditor(package,palettes)
        gallery=OUT/"graphics"
        gallery.mkdir(parents=True,exist_ok=True)
        for ns,(source,style,default) in REGIONS[code].items():
            index=int(ns)
            asset=f"graphic/{code}/{index}"
            image=package.render(editor.frames[index],palettes)
            shot=BytesIO();image.save(shot,format="PNG")
            ctx=resolver.catalog.context(game,scene,proc,lr,"graphic",source,None)
            ctx.update(package=code,frame_index=index,width_px=image.width,height_px=image.height,
                       palette_banks=sorted({p["palette"] for p in package.parts(editor.frames[index])}),
                       typography=style)
            image.save(gallery/f"{code}-{index}-ja.png")
            resolver.memory.answers[asset]=default
            if resolver.enabled:
                english,origin,event=resolver.memory.choose(asset,source,ctx,shot.getvalue())
                if english:
                    converted=lettering(image,english,style)
                    editor.replace(index,converted)
            else:
                english,origin,event=None,None,"japanese"
            resolver.record(asset,source=source,english=english,provenance=origin,event=event,frame=frame,**ctx)
        if not editor.changed: return 0
        translated=editor.pack()
        result=sprites.Package(translated)
        frames=list(result.frames())
        for ns in REGIONS[code]:
            index=int(ns)
            result.render(frames[index],palettes).save(gallery/f"{code}-{index}-en.png")
        self.loaded[key]=game.install(translated)
        return self.loaded[key]
