#include "global.h"
#define Actor_ConfigureFromScript sub_081EA1C0
// Provisional name: Actor_ConfigureFromScript
extern u8 *gUnk_020003C8;
u8 *sub_081EA730(s32, s32);
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
    u8 p6b[2];
    u16 resource;
    void (*previous)(void *);
    void (*fn)(void *);
    void (*scaleFn)(void *);
    void (*otherFn)(void *);
    s32 timer;
    u32 variant;
};
extern u8 gUnk_03005430[];
void sub_08215284(void *, s32);
void sub_08013440(void *);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(s32);
s32 sub_0821ABA8(s32, s32);
#define B(o) (*(u8 *)((u8 *)g + (o)))
#define H(o) (*(u16 *)((u8 *)g + (o)))
#define W(o) (*(u32 *)((u8 *)g + (o)))
static inline void state(struct G *g, u32 mode, u32 phase)
{
    g->oldMode = g->mode;
    g->mode = mode;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | (phase << 4);
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}
void Actor_ConfigureFromScript(void)
{
    struct G *g = (struct G *)gUnk_020003C8;
    struct Obj *obj;
    if (!g)
        g = (struct G *)sub_081EA730(0xbb34, 0);
    obj = (struct Obj *)((u8 *)g + 0xddc);
    sub_08215284((u8 *)g + 0xe08, 343);
    g->obj.priority = 0;
    g->last = 0;
    g->resource = Script_GetValue();
    g->next = sub_0821ABA8(0x72, 0);
    g->gap6a = sub_0821ABA8(0x41, 0);
    if (Script_SeekToKeyword(0x70)) {
        {
            s32 v = Script_GetValue();
            g->savedX = v;
            obj->x = v;
        }
        {
            s32 v = Script_GetValue();
            g->savedY = v;
            obj->y = v;
        }
        {
            s32 v = Script_GetValue();
            g->savedZ = v;
            obj->z = v;
        }
    } else {
        u8 *owner;
        owner = *(u8 **)((u8 *)g + 0x18);
        g->savedX = *(u16 *)(owner + 0x30);
        obj->x = *(u16 *)(owner + 0x30);
        owner = *(u8 **)((u8 *)g + 0x18);
        g->savedY = *(u16 *)(owner + 0x32);
        obj->y = *(u16 *)(owner + 0x32);
        owner = *(u8 **)((u8 *)g + 0x18);
        g->savedZ = *(u16 *)(owner + 0x34);
        obj->z = *(u16 *)(owner + 0x34);
    }
    if (Script_SeekToKeyword(0x63))
        W(0xf50) = Script_GetValue();
    g->gap65 = sub_0821ABA8(0x73, 0);
    if (gUnk_03005430[14] != 5 && gUnk_03005430[14] != 0 && gUnk_03005430[14] != 4)
        g->p6a[1] = 1;
    else
        g->p6a[1] = 0;
    sub_08013440((u8 *)g + 0xe34);
    *(u32 *)((u8 *)g + 0xddc) |= 1;
    switch (g->gap65) {
        case 0:
            state(g, 0, 0);
            break;
        case 1:
            state(g, 2, 1);
            break;
        case 2:
            state(g, 2, 2);
            break;
        case 3:
            state(g, 2, 3);
            break;
        case 4:
            state(g, 2, 5);
            break;
        case 5:
            state(g, 2, 6);
            break;
    }
}
