"""Translate ink in CB05 sprite frames; preserve animation keys and unrelated art.

Each edited frame gets its own part list and 4bpp tile rectangles. This matters
for menu resources where a single Japanese glyph tile appears in several labels.
"""
from collections import Counter
from functools import lru_cache
import json
import struct
from PIL import Image, ImageDraw, ImageFont, ImageFilter
from runtime import HERE
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


def check_letter_width(text, width):
    if "\n" in text:
        raise ValueError("Use one line for this sprite label")
    # Reserve at least two pixels of horizontal ink per non-space character.
    if sum(1 if char == " " else 2 for char in text) > width:
        raise ValueError(f"Shorten the sprite label to fit its {width}-pixel ink area")


def pixels(image):
    if hasattr(image, "get_flattened_data"):
        return image.get_flattened_data()
    return image.getdata()


def letter_mask(text, width, height, heavy=False):
    if not text.strip() or width < 1 or height < 1:
        raise ValueError("Empty graphic or empty lettering")
    check_letter_width(text, width)
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
        # The widget extends past x=240. Keep the translated ink inside the
        # original wordmark's ink bounds, including the outline and extrusion.
        left, top, right, bottom = source.getbbox()
        ink = out.crop(out.getbbox())
        ink.thumbnail((right-left, bottom-top), Image.Resampling.NEAREST)
        aligned = Image.new("RGBA", out.size)
        aligned.paste(ink, (left+(right-left-ink.width)//2, top+(bottom-top-ink.height)//2))
        return aligned
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
    colors=Counter(rgb[:3] for rgb in pixels(im) if rgb[3])
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
    return best[1], [best[2].get(rgb[:3],0) if rgb[3] else 0 for rgb in pixels(im)]
