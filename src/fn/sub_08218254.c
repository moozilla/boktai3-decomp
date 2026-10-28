#include "global.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"
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
static inline s32 UnsignedScale(u8 v)
{
    return v;
}
static inline s32 divide8(s32 input)
{
    return input >= 0 ? input >> 3 : -((-input) >> 3);
}
static inline s32 divide2(s32 input)
{
    return input >= 0 ? input >> 1 : -((-input) >> 1);
}
void SpriteOam_QueueCameraRelativeSprites(void)
{

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
        register u32 flags asm("r6");
        register u32 attr asm("r5");
        s32 depth;
        s32 ox, oy;
        register u32 initialFlags asm("r1") = p->flags;
        u32 t = initialFlags;
        asm("" : "+r"(t));
        t &= skip;
        flags = initialFlags;
        if (t)
            goto next;

        x = divide8(p->pos.x - gCamera.position.x) + 120;
        y = divide8(p->pos.y - gCamera.position.y) + 80;
        {
            register s32 result asm("r0") = p->pos.z - gCamera.position.z;
            z = result;
        }
        {
            u32 cull = flags & 64;
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
                gSpriteAffineParams[*count].angle = angle;
                gSpriteAffineParams[*count].scaleX = sx;
                gSpriteAffineParams[*count].scaleY = sy;
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
