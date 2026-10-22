#include "global.h"

#ifdef NONMATCHING
// fuzzy: 99.1%
// Left: in the piece loop preheader the ROM initialises the attrs[] giv before the
// hoisted flags & 8 (6 instructions in a different order).

struct SpritePiece {
    u8 shape;
    u8 pad1;
    s8 x, y;
};
struct SpriteFrame {
    u8 count;
    u8 tileSize;
    u8 pad2[2];
    u32 tileOffset;
    u32 pieceOffset;
};
struct SpriteRes {
    u8 pad0[6];
    u16 paletteKey;
    u8 pad8[4];
    void *palette;
    u8 pad10[4];
    struct SpriteFrame *frames;
};
struct SpriteAnim {
    s16 frame;
    u8 pad2;
    u8 paletteOffset;
    u8 pad4[2];
    s8 x, y;
};
struct Sprite {
    u32 flags;
    u8 pad4[2];
    u8 rotation;
    u8 priority;
    s8 scaleX, scaleY;
    u8 padA[2];
    struct SpriteRes *resource;
    struct SpriteAnim anim;
    u32 attributes;
};
struct SpriteAffine {
    u8 rotation;
    s8 scaleX, scaleY;
    u8 pad;
};
struct OamEntry {
    u32 attributes;
    u16 tile;
    u16 depth;
};

extern u8 gUnk_03005230;
extern u32 gUnk_0300522C;
extern s32 gUnk_03004BA0;
extern struct SpriteAffine gUnk_03004310[];
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_030042FC;
extern u8 *gUnk_030042F8;
extern u8 gUnk_03004D90[], gUnk_03004D20[], gUnk_03004D30[];
extern u32 gUnk_03004D50[];
extern s32 gUnk_03004798, gUnk_0300479C;
extern struct OamEntry gUnk_03004390[];
extern struct OamEntry gUnk_030047A0[];
extern u16 gUnk_03004790[];
extern u32 gUnk_03003A04;
s32 sub_082152E4(const void *src, s32 size);
u32 Palette_CacheObjectBank(u32 key, void *src);

static inline s32 DIV64(s32 v)
{
    return v >= 0 ? v >> 6 : -((-v) >> 6);
}
static inline s32 DIV4096(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}

static inline void Oam_PushImmediate(u32 a, u32 t)
{
    s32 n = gUnk_03004798;
    if (n <= 127) {
        gUnk_03004390[n].attributes = a;
        gUnk_03004390[n].tile = t;
        gUnk_03004798 = n + 1;
        gUnk_03003A04 |= 1;
    }
}
static inline void Oam_PushSorted(u32 a, u32 t, s32 z, u32 priority)
{
    s32 n = gUnk_0300479C;
    if (n <= 127) {
        struct OamEntry *e = &gUnk_030047A0[n];
        e->attributes = a;
        e->tile = t;
        e->depth = ((z + 128) & 0xFF) << 8;
        gUnk_0300479C = n + 1;
        gUnk_03004790[priority]++;
    }
}
static inline s32 Affine_Alloc(u8 rotation, s8 sx, s8 sy)
{
    s32 n;
    if (gUnk_03004BA0 > 31)
        return 0;
    gUnk_03004310[gUnk_03004BA0].rotation = rotation;
    gUnk_03004310[gUnk_03004BA0].scaleX = sx;
    gUnk_03004310[gUnk_03004BA0].scaleY = sy;
    n = gUnk_03004BA0++;
    return n;
}

void sub_08214600(struct Sprite *p, s32 x, s32 y, s32 z)
{
    u32 attrs[32];
    s32 tiles[32];
    s32 k;
    s32 sinv, cosv;
    u32 attr;
    s32 count;
    struct SpriteRes *res;
    s32 tile, prev;
    s32 baseX, baseY;
    u32 priority;
    struct SpriteAnim *anim;
    struct SpriteFrame *frame;
    struct SpritePiece *piece;

    z = z >= 0 ? z >> 1 : -((-z) >> 1);
    if (z < -128)
        z = -128;
    else if (z > 127)
        z = 127;
    attr = 0;
    if (((gUnk_03005230 & 1) && p->priority != 0 && !(p->flags & 0x10000)) || (p->flags & 0x800))
        attr |= 0x1000;
    if (p->flags & 0x100)
        attr |= 0x800;
    if (gUnk_0300522C != 0 && (p->flags & 0x4000))
        attr |= 0x400;
    attr |= p->attributes;
    if (!(p->flags & 2)) {
        if (p->flags & 4)
            attr |= 0x10000000;
        if (p->flags & 8)
            attr |= 0x20000000;
        sinv = 0;
        cosv = 0;
    } else {
        s32 sx = p->scaleX;
        s32 sy = p->scaleY;
        if (p->flags & 4)
            sx = -sx;
        if (p->flags & 8)
            sy = -sy;
        if (p->flags & 0x20000)
            attr |= 0x100;
        else if ((sx >= 0 ? sx : -sx) > 60 || (sy >= 0 ? sy : -sy) > 60 || p->rotation != 0)
            attr |= 0x300;
        else
            attr |= 0x100;
        attr |= Affine_Alloc(p->rotation, sx, sy) << 25;
        sinv = gUnk_086149C4[-p->rotation & 0xFF];
        cosv = gUnk_086149C4[(-p->rotation + 0x40) & 0xFF];
    }
    res = p->resource;
    anim = &p->anim;
    frame = res->frames;
    frame += anim->frame;
    {
        u8 *pieces = gUnk_030042FC + frame->pieceOffset;
        if (p->flags & 4)
            x -= anim->x;
        else
            x += anim->x;
        baseX = x;
        if (p->flags & 8)
            baseY = y - anim->y;
        else
            baseY = y + anim->y;
        priority = p->priority;
        count = 0;
        tile = 0;
        prev = 0;
        piece = (struct SpritePiece *)pieces;
    }
    for (k = 0; k < frame->count; k++, piece++) {
        u32 shape = piece->shape;
        s32 w, h, left, top;
        tile += prev;
        w = gUnk_03004D90[shape];
        h = gUnk_03004D20[shape];
        prev = gUnk_03004D30[shape];
        if (!(p->flags & 2)) {
            s32 right, bottom;
            if (p->flags & 4) {
                right = -piece->x;
                left = right - w;
            } else {
                left = piece->x;
                right = left + w;
            }
            if (p->flags & 8) {
                bottom = -piece->y;
                top = bottom - h;
            } else {
                top = piece->y;
                bottom = top + h;
            }
            left += baseX;
            top += baseY;
            if (left > 240 || top > 160 || right + baseX < 0 || bottom + baseY < 0)
                continue;
        } else {
            s32 hw = w >> 1;
            s32 hh = h >> 1;
            s32 dx, dy, v;
            v = (piece->x + hw) * p->scaleX;
            dx = DIV64(v);
            v = (piece->y + hh) * p->scaleY;
            dy = DIV64(v);
            if (p->flags & 4)
                dx = -dx;
            if (p->flags & 8)
                dy = -dy;
            v = dx * cosv - dy * sinv;
            left = DIV4096(v) - hw;
            v = dx * sinv + dy * cosv;
            top = DIV4096(v) - hh;
            if ((attr & 0x300) == 0x300) {
                left -= hw;
                top -= hh;
                w <<= 1;
                h <<= 1;
            }
            left += baseX;
            top += baseY;
            if (left + w < 0 || left > 240 || top + h < 0 || top > 160)
                continue;
        }
        attrs[count] = ((left & 0x1FF) << 16) | (top & 0xFF) | attr | gUnk_03004D50[shape];
        tiles[count] = tile;
        count++;
    }
    if (count > 0) {
        s32 base = sub_082152E4(gUnk_030042F8 + frame->tileOffset, frame->tileSize);
        u32 extra =
            (priority << 10) |
            (Palette_CacheObjectBank(res->paletteKey + anim->paletteOffset, res->palette) << 12);
        if (p->flags & 0x20) {
            for (k = 0; k < count; k++)
                Oam_PushImmediate(attrs[k], (base + tiles[k]) | extra);
        } else {
            for (k = 0; k < count; k++)
                Oam_PushSorted(attrs[k], (base + tiles[k]) | extra, z, priority);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08214600);
#endif
