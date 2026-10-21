#include "global.h"
// MATCHING_CONCESSION: register-pinning
// Remaining differences: projection source register and vertical-bound operand allocation.
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
static inline void screen(struct Vec *dst, struct Sprite *sp)
{
    register u32 off asm("r0") = 0x1c;
    register struct Vec *t asm("r0");
    register struct Vec *src asm("r12");
    s32 depth, height, y, z;
    s32 sx, sz;
    asm("" : "+r"(off));
    off += (u32)sp;
    t = (struct Vec *)off;
    src = t;
    sx = sp->pos.x;
    asm("" ::"r"(src));
    asm("" : "+r"(t));
    sz = t->z;
    dst->x = project(sx - sz);
    depth = project(sx + sz);
    {
        register struct Vec *q asm("r2") = src;
        height = project(q->y);
    }
    y = depth - height;
    z = depth + height;
    dst->x = dst->x - gUnk_03005418[0] + 120;
    dst->y = y - gUnk_03005418[1] + 80;
    dst->z = z - gUnk_03005418[2];
}
// confidence=low: resource flag selects this list; name describes observed bounds handling.
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
            screen(&pos, p);
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
            s32 left;
            register s32 top asm("r4");
            register u32 width asm("r12");
            register u32 height asm("r1");
            register s32 bot asm("r0");
            if (p->flags & 4) {
                register s32 edge asm("r0") = x + resource->offsetX;
                left = edge - resource->width;
            } else {
                left = x - resource->offsetX;
            }
            width = resource->width;
            if (p->flags & 8) {
                register s32 t asm("r0") = y + resource->offsetY;
                top = t - resource->height;
                height = resource->height;
            } else {
                register s32 t asm("r0") = resource->offsetY;
                top = y - t;
                height = resource->height;
            }
            if (left > 240 || top > 160 || left + (s32)width < 0 || (bot = top + (s32)height) < 0)
                goto next;
        }
        sub_08214600(p, x, y, z);
    next:
        p = p->next;
    } while (p);
}
