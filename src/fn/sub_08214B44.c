#include "global.h"
#include "render.h"
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
    u8 pad4[4];
    s8 scaleX, scaleY;
    u8 padA[2];
    struct SpriteResource *resource;
    u8 pad10[12];
    struct Vec pos;
    u32 previous;
    struct Sprite *next;
};
extern u32 gUnk_03003A50;
extern struct Sprite *gUnk_030042C0[];
extern u16 gUnk_03005418[];
void sub_08214600(struct Sprite *, s32, s32, s32);
static inline s32 project(s32 input)
{
    s32 v = input * 3;
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
static inline void screen(struct Vec *dst, struct Vec *src)
{
    s32 depth, height, y, z;
    s32 sx = src->x, sz = src->z;
    dst->x = project(sx - sz);
    depth = project(sx + sz);
    height = project(src->y);
    y = depth - height;
    z = depth + height;
    dst->x = dst->x - gUnk_03005418[0] + 120;
    dst->y = y - gUnk_03005418[1] + 80;
    dst->z = z - gUnk_03005418[2];
}
// Applies signed sprite scales and flip flags when testing resource bounds.
void Sprite_RenderScaledList(void)
{

    s32 z = 0;
    struct Vec pos;
    struct Sprite *p;
    u32 parity = gUnk_03003A50 & 1;
    u32 skip = 0x401;
    if (parity) {
        skip = 0x201;
    }
    do {
        p = gUnk_030042C0[gRenderListIndex];
        if (!p) {
            return;
        }
    } while (0);
    do {
        s32 x;
        s32 y;
        struct SpriteResource *resource;
        if (p->flags & skip) {
            goto next;
        }
        resource = p->resource;
        if (!(p->flags & 16)) {
            screen(&pos, &p->pos);
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = p->pos.x;
            y = p->pos.y;
            z = p->pos.z;
        }
        if (!(p->flags & 64)) {
            s32 left, top;
            u32 width, height;
            if (p->flags & 2) {
                s32 ox = (resource->offsetX * p->scaleX) >> 6;
                s32 oy = (resource->offsetY * p->scaleY) >> 6;
                width = (resource->width * p->scaleX) >> 6;
                height = (resource->height * p->scaleY) >> 6;
                if (p->flags & 4) {
                    left = x + ox - width;
                } else {
                    left = x - ox;
                }
                if (p->flags & 8) {
                    top = y + oy - height;
                } else {
                    top = y - oy;
                }
            } else {
                width = resource->width;
                height = resource->height;
                if (p->flags & 4) {
                    left = x + resource->offsetX - width;
                } else {
                    left = x - resource->offsetX;
                }
                if (p->flags & 8) {
                    top = y + resource->offsetY - height;
                } else {
                    top = y - resource->offsetY;
                }
            }
            if ((((left > 240) || (top > 160)) || ((left + ((s32)width)) < 0)) ||
                ((top + ((s32)height)) < 0)) {
                goto next;
            }
        }
        sub_08214600(p, x, y, z);
    next:
        p = p->next;
    } while (p);
}
