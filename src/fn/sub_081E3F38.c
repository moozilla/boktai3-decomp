#include "global.h"
// Provisional name: Actor_SelectNearestInteraction
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
    u8 timer;
    u8 gap3[0xd0 - 0xb5];
    u16 resource;
    u8 gap4[2];
    s32 distance;
    u8 eligible;
    u8 gap5[3];
};
extern u8 *gUnk_02000710;
extern u16 gUnk_03005260[];
extern u32 gUnk_0300523C;
u8 sub_082215E4(s32, s32);
void sub_0821AD08(void *, s32);
void sub_081E574C(void *, void *);
void sub_081E5648(void *, void *);
void sub_081E4174(void *);
void sub_081E44D4(void *);
void sub_08215284(void *, s32);
// Select the closest eligible interaction target and animate its selection marker.
void Actor_SelectNearestInteraction(struct G *p)
{
    s32 i;
    struct Partner *q;
    s32 dx, dz, xOffset, zOffset, length;
    register s32 height asm("r0");
    u32 radius;
    u32 a, b, c;
    u32 sector;
    u8 *owner;
    p->group = -1;
    for (i = 0; i < 16; i++) {
        q = (struct Partner *)((u8 *)p + 0x1c + i * 0xdc);
        q->eligible = 0;
        if (q->flags & 10)
            continue;
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
            continue;
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
            u32 angle = (u8)(sub_082215E4(dx, dz) - 16);
            register s32 first asm("r1") = (s8)a;
            sector = angle >> 5;
            if (first != sector && (s8)b != sector && (s8)c != sector) {
                c = 255;
                b = 255;
                a = 255;
            }
        }
        if ((s8)a != *(s16 *)(gUnk_02000710 + 0x85e) && (s8)b != *(s16 *)(gUnk_02000710 + 0x85e) &&
            (s8)c != *(s16 *)(gUnk_02000710 + 0x85e))
            continue;
        height = q->worldY;
        {
            u8 *parent = *(u8 **)((u8 *)p + 0x18);
            height -= *(s16 *)(parent + 0x32);
        }
        if (height < 0)
            height = -height;
        if (height > 119)
            continue;

        q->eligible = 1;
        q->distance = dx * dx + dz * dz;
        if (p->group == -1)
            p->group = i;
        else {
            s32 offset = p->group * 0xdc;
            u8 *base = (u8 *)p + 0xf0;
            if (q->distance < *(s32 *)(base + offset))
                p->group = i;
        }
    }
    for (i = 0; i < 16; i++) {
        q = (struct Partner *)((u8 *)p + 0x1c + i * 0xdc);
        if (i == p->group) {
            if (++q->timer > 10) {
                sub_08215284((u8 *)q + 0x2c, q->resource);
                q->timer = 10;
            } else
                sub_08215284((u8 *)q + 0x2c, q->resource + 1);
        } else {
            u8 old = q->timer--;
            if (old == 0) {
                q->timer = 0;
                sub_08215284((u8 *)q + 0x2c, q->resource + 2);
            } else
                sub_08215284((u8 *)q + 0x2c, q->resource + 1);
        }
    }
}
