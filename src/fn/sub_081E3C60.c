#include "global.h"
// Provisional name: Actor_TryInteraction
// MATCHING_CONCESSION: register-pinning
struct Obj {
    u32 flags;
    u8 pad[3];
    u8 priority;
    s8 scaleX, scaleY;
    u8 padScale[6];
    u16 frame;
    u8 pad2[10];
    s16 x, y, z;
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct G {
    u8 p0[0xddc];
    struct Obj obj;
    u8 p1a[0xe08 - 0xe00];
    u8 effect[0xe24 - 0xe08];
    struct Anim anim;
    u8 p2[0xe74 - 0xe34];
    u32 arg;
    u16 count;
    s8 delta;
    u8 p3a[0xec8 - 0xe7b];
    s16 deltaX, deltaY;
    u8 gapCC[4];
    s16 targetX, targetY, targetZ;
    u8 gapD6[2];
    s16 startX, startY, startZ;
    u8 gapDE[2];
    s16 savedX, savedY, savedZ;
    u8 gapE6[0xf50 - 0xee6];
    void *handle;
    u8 p3b[4];
    u8 done, state;
    u16 mode : 4;
    u16 phase : 12;
    u16 oldMode;
    u16 speed, heading;
    u8 gap62[2];
    u8 flags;
    u8 gap65;
    u8 direction;
    u8 gap67[2];
    s8 bend;
    u8 gap6a;
    u8 flip;
    u8 last, next;
    u8 p6a[0xf86 - 0xf6e];
    u8 savedFrame, savedTimer;
    u8 p6b;
    s8 group;
    u16 resource;
    void (*previous)(void *);
    void (*fn)(void *);
    void (*scaleFn)(void *);
    void (*otherFn)(void *);
    s32 timer;
    u32 variant;
};

struct Partner {
    u8 pad[0x1c];
    s16 x, y, z;
    u8 gap[0x48 - 0x22];
    s16 worldX, worldY, worldZ;
    u8 gap2[0xa4 - 0x4e];
    void *handle, *request;
    void (*fn)(void *, void *);
    u8 group, flags, orientation, option;
};
extern u8 *gUnk_02000710;
extern u16 gUnk_03005260[];
extern u32 gUnk_0300523C;
u8 Math_Atan2(s32, s32);
void Script_Call(void *, s32);
void sub_081E574C(void *, void *);
void sub_081E5648(void *, void *);
void sub_081E4174(void *);
void sub_081E44D4(void *);
static inline u32 mode_bits(u8 *p, u32 mask)
{
    register u32 v asm("r1") = *p;
    u32 result = mask & v;
    return result;
}
// Tentative: accept a nearby interaction when facing, height and input permit it.
void Actor_TryInteraction(struct G *p, struct Partner *q)
{
    s32 dx, dz, xOffset, height, length;
    s32 zOffset;
    u32 radius;
    u32 a;
    u32 b, c;
    u32 sector;
    u8 *owner;
    if (q->group != p->group)
        return;
    if (q->orientation == 0) {
        xOffset = 0;
        zOffset = 128;
    } else {
        xOffset = 128;
        zOffset = 0;
    }
    owner = *(u8 **)((u8 *)p + 0x18);
    dx = *(s16 *)(owner + 0x30) - q->worldX - xOffset;
    dz = *(s16 *)(owner + 0x34) - q->worldZ - zOffset;
    length = dx * dx + dz * dz;
    radius = p->speed;
    if (length >= (s32)(radius * radius))
        return;
    if (q->orientation == 0) {
        a = 0;
        b = 6;
        c = 7;
    } else {
        a = 0;
        b = 1;
        c = 2;
    }
    {
        u32 angle = (u8)(Math_Atan2(dx, dz) - 16);
        s32 first = (s8)a;
        sector = angle >> 5;
        if (first != sector && (s8)b != sector && (s8)c != sector) {
            c = 255;
            b = 255;
            a = 255;
        }
    }
    if ((s8)a != *(s16 *)(gUnk_02000710 + 0x85e) && (s8)b != *(s16 *)(gUnk_02000710 + 0x85e) &&
        (s8)c != *(s16 *)(gUnk_02000710 + 0x85e))
        return;
    height = q->worldY;
    owner = *(u8 **)((u8 *)p + 0x18);
    height -= *(s16 *)(owner + 0x32);
    if (height < 0)
        height = -height;
    if (height > 119)
        return;
    if (!(gUnk_03005260[1] & 1))
        return;
    if (gUnk_0300523C & 4)
        return;
    if (*(u32 *)(owner + 0x1c) == 4)
        return;
    if (q->request)
        Script_Call(q->request, 0);
    p->obj.x = q->x;
    p->obj.y = q->y;
    p->obj.z = q->z;
    p->handle = q->handle;
    p->gap6a = q->option;
    if (q->flags & 16) {
        p->oldMode = p->mode;
        {
            u32 mode = 1;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 0x30;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register u32 one asm("r1") = 1;
            register u32 flags asm("r2") = p->flags;
            p->flags = one | flags;
            {
                register void (*fn)(void *, void *) asm("r2") = sub_081E574C;
                q->fn = fn;
            }
            one |= q->flags;
            q->flags = one;
        }
        sub_081E4174(p);
    } else {
        p->oldMode = p->mode;
        {
            u32 mode = 1;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 0x20;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register u32 one asm("r1") = 1;
            register u32 flags asm("r2") = p->flags;
            p->flags = one | flags;
            {
                register void (*fn)(void *, void *) asm("r2") = sub_081E5648;
                q->fn = fn;
            }
            one |= q->flags;
            q->flags = one;
        }
        sub_081E44D4(p);
    }
}
