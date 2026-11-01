// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
#include "ram.h"
struct Vec {
    s16 x, y, z;
};
struct SpriteResource {
    u8 pad[8];
    u8 width, height;
    s8 offsetX, offsetY;
};
struct Sprite {
    u32 flags;
    u8 pad4[8];
    struct SpriteResource *resource;
    u8 pad10[12];
    struct Vec pos;
    u32 previous;
    struct Sprite *next;
};
void sub_08214600(struct Sprite *, s32, s32, s32);
static inline s32 project(s32 input)
{
    s32 v = input * 3;
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
static inline void screen(struct Vec *dst, struct Sprite *sp)
{
    u32 off = 0x1c;
    struct Vec *t;
    struct Vec *src;
    s32 depth, height, y, z;
    s32 sx, sz;
    off += (u32)sp;
    t = (struct Vec *)off;
    src = t;
    sx = sp->pos.x;
    sz = t->z;
    dst->x = project(sx - sz);
    depth = project(sx + sz);
    {
        struct Vec *q = src;
        height = project(q->y);
    }
    y = depth - height;
    z = depth + height;
    dst->x = dst->x - gCameraOrigin[0] + 120;
    dst->y = y - gCameraOrigin[1] + 80;
    dst->z = z - gCameraOrigin[2];
}
// confidence=low: resource flag selects this list; name describes observed bounds handling.
void Sprite_RenderFlipAwareList(void)
{
    struct Vec pos;
    struct Sprite *p;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity) {
        skip = 0x201;
    }
    p = gSpriteListHeads[gRenderListIndex];
    if (!p) {
        return;
    }
    do {
        s32 x, y, z;
        struct SpriteResource *resource;
        if (p->flags & skip) {
            goto next;
        }
        if (!(p->flags & 0x1000)) {
            goto next;
        }
        resource = p->resource;
        if (!(p->flags & 16)) {
            screen(&pos, p);
            {
                struct Vec *out = &pos;
                DEBUG_NOP();
                x = out->x;
                y = out->y;
                z = out->z;
            }
        } else {
            x = p->pos.x;
            y = p->pos.y;
            z = p->pos.z;
        }
        if (!(p->flags & 64)) {
            s32 left;
            s32 top;
            u32 width;
            u32 height;
            s32 bot;
            if (p->flags & 4) {
                s32 edge = x + resource->offsetX;
                left = edge - resource->width;
            } else {
                left = x - resource->offsetX;
            }
            width = resource->width;
            if (p->flags & 8) {
                s32 t = y + resource->offsetY;
                top = t - resource->height;
                height = resource->height;
            } else {
                short t = resource->offsetY;
                top = y - t;
                height = resource->height;
            }
            if ((((left > 240) || (top > 160)) || ((left + ((s32)width)) < 0)) ||
                ((bot = top + ((s32)height)) < 0)) {
                goto next;
            }
        }
        sub_08214600(p, x, y, z);
    next:
        p = p->next;
    } while (p);
}
