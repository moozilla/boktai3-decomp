#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 89.1%
// Projection-source allocation and flipped-bounds operand scheduling still differ.
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
extern u32 gUnk_03003A50, gUnk_03004290;
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
// confidence=low: resource flag selects this list; name describes observed bounds handling.
#define Sprite_RenderFlipAwareList sub_08214E3C
void Sprite_RenderFlipAwareList(void)
{
    struct Vec pos;
    struct Sprite *p;
    u32 parity = gUnk_03003A50 & 1;
    u32 skip = 0x401;
    if (parity)
        skip = 0x201;
    p = gUnk_030042C0[gUnk_03004290];
    if (!p)
        return;
    do {
        register s32 x asm("r3"), y asm("r7"), z asm("r8");
        struct SpriteResource *resource;
        if (p->flags & skip)
            goto next;
        if (!(p->flags & 0x1000))
            goto next;
        resource = p->resource;
        if (!(p->flags & 16)) {
            screen(&pos, &p->pos);
            {
                register struct Vec *out asm("r0") = &pos;
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
            s32 left, top;
            register u32 width asm("r12");
            u32 height;
            if (p->flags & 4) {
                left = x + resource->offsetX - resource->width;
            } else {
                left = x - resource->offsetX;
            }
            width = resource->width;
            if (p->flags & 8) {
                top = y + resource->offsetY - resource->height;
                height = resource->height;
            } else {
                top = y - resource->offsetY;
                height = resource->height;
            }
            if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0)
                goto next;
        }
        sub_08214600(p, x, y, z);
    next:
        p = p->next;
    } while (p);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08214E3C);
#endif
