#include "global.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"
// confidence=low: flag 0x2000 selects this sprite staging pass.
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z;
};
struct Drawable {
    u32 flags;
    u8 f4, scaleX, scaleY, angle;
    s16 tile;
    u8 width, height;
    s8 offsetX, offsetY;
    u8 palette, priority;
    s8 depthOffset;
    u8 pad11[3];
    u32 attributes;
    struct Vec pos;
    u8 pad1e[4];
    struct Drawable *next;
};
struct Matrix {
    u8 angle;
    s8 scaleX, scaleY;
    u8 pad;
};
extern struct Matrix gUnk_03004310[];
extern u16 gUnk_03005418[];
static inline s32 UnsignedScale(u8 v)
{
    return v;
}
static inline s32 project(s32 input)
{
    s32 v = input * 3;
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
static inline s32 divide2(s32 input)
{
    return input >= 0 ? input >> 1 : -((-input) >> 1);
}
void SpriteOam_QueueOverlayProjectedSprites(void)
{
    struct Vec pos;
    struct Drawable *p;
    s32 *matrixCount;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity)
        skip = 0x201;
    p = gDrawableListHeads[gRenderListIndex];
    if (!p)
        return;
    matrixCount = &gAffineRequestCount;
    do {
        s32 x, y;
        s32 z;
        u32 flags;
        u32 attr;
        s32 depth;
        s32 ox, oy;
        u32 initialFlags = p->flags;
        if (initialFlags & skip)
            goto next;
        if (!(initialFlags & 0x2000))
            goto next;

        if (!(initialFlags & 16)) {
            {
                register struct Vec *out asm("r5") = &pos;
                struct Vec *in = &p->pos;
                s32 depth, height, y, z;
                s32 sx = p->pos.x, sz = in->z;
                out->x = project(sx - sz);
                depth = project(sx + sz);
                height = project(in->y);
                y = depth - height;
                z = depth + height;
                out->x = out->x - gUnk_03005418[0] + 120;
                out->y = y - gUnk_03005418[1] + 80;
                out->z = z - gUnk_03005418[2];
            }
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = p->pos.x;
            y = p->pos.y;
            z = p->pos.z;
        }
        {
            u32 loaded = p->flags;
            u32 cull = loaded & 64;
            flags = loaded;
            ox = p->offsetX;
            oy = p->offsetY;
            if (!cull) {
                u32 width = p->width, height = p->height;
                s32 left = x + ox, top = y + oy;
                if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0)
                    goto next;
            }
        }
        x += ox;
        y += oy;
        depth = z;
        x &= 511;
        {
            u32 shifted = x << 16;
            attr = p->attributes | shifted;
        }
        y &= 255;
        attr |= y;
        if (flags & 2) {
            u32 matrix;
            u32 angle;
            s32 sx, sy;
            if (!(flags & 0x20000) &&
                (UnsignedScale(p->scaleX) > 60 || UnsignedScale(p->scaleY) > 60 || p->angle != 0))
                attr |= 0x300;
            else
                attr |= 0x100;
            angle = p->angle;
            sx = (s8)p->scaleX;
            sy = (s8)p->scaleY;
            if (*matrixCount > 31)
                matrix = 0;
            else {
                s32 *count = matrixCount;
                gUnk_03004310[*count].angle = angle;
                gUnk_03004310[*count].scaleX = sx;
                gUnk_03004310[*count].scaleY = sy;
                {
                    register s32 old asm("r0") = *count;
                    matrix = old;
                    ++old;
                    *count = old;
                }
            }
            attr |= matrix << 25;
            flags = p->flags;
        }
        if (flags & 4)
            attr |= 0x10000000;
        if (flags & 8)
            attr |= 0x20000000;
        if (gSpriteAttrFlags && (flags & 0x4000))
            attr |= 0x400;
        if (((gSpriteMosaicAttr & 1) && p->priority && !(flags & 0x10000)) || (flags & 0x800))
            attr |= 0x1000;
        depth += p->depthOffset;
        if (flags & 0x20) {
            s32 tile = p->tile | (p->priority << 10) | (p->palette << 12);
            s32 n = gOamCount;
            if (n > 127)
                goto next;
            gOamBuffer[n].attributes = attr;
            gOamBuffer[n].tile = tile;
            gOamCount = n + 1;
            gVideoUploadFlags |= 1;
        } else {
            s32 tile, n;
            u32 priority;
            depth = divide2(depth);
            if (depth < -128)
                depth = -128;
            else if (depth > 127)
                depth = 127;
            tile = p->tile;
            priority = p->priority;
            tile = tile | (priority << 10) | (p->palette << 12);
            n = gPendingOamCount;
            if (n > 127)
                goto next;
            gPendingOamBuffer[n].attributes = attr;
            gPendingOamBuffer[n].tile = tile;
            gPendingOamBuffer[n].depth = ((depth + 128) & 255) << 8;
            gPendingOamCount = n + 1;
            gOamPriorityCounts[priority]++;
        }
    next:
        p = p->next;
    } while (p);
}
