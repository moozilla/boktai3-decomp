#include "global.h"

#ifdef NONMATCHING
// fuzzy: 63.9%
// Directional pair placement recovered; stack-vector allocation and projection ordering differ.

struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16;
    u32 pad : 16;
};
extern u8 *gUnk_02000710;
extern struct Vec gUnk_03005418;
void Player_SetDirectionalEffectFrame(void *, void *, s32);

static inline struct Vec *SetVec(struct Vec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}

static inline s32 Scale(s32 value)
{
    value *= 3;
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}

static inline void Project(struct Vec *screen, struct PackedVec *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 depth, height;
    screen->x = Scale(x - z);
    depth = Scale(x + z);
    height = Scale(world->y);
    screen->x = (u16)screen->x - (u16)gUnk_03005418.x + 120;
    screen->y = depth - height - (u16)gUnk_03005418.y + 80;
    screen->z = depth + height - (u16)gUnk_03005418.z;
}

static inline void Add(struct Vec *out, struct Vec *a, struct Vec *b)
{
    out->x = (u16)a->x + (u16)b->x;
    out->y = (u16)a->y + (u16)b->y;
    out->z = (u16)a->z + (u16)b->z;
}

void sub_0813BAD8(u8 *p)
{
    struct Vec first, second;
    struct PackedVec position;
    struct Vec projected;
    s32 direction;
    s32 height, depth;
    if (p[0x7C8] != 1) {
        *(u32 *)(p + 0x778) |= 1;
        *(u32 *)(p + 0x7A0) |= 1;
        return;
    }
    p[0x7C8] = 0;
    *(u32 *)(p + 0x778) &= ~1;
    *(u32 *)(p + 0x7A0) &= ~1;
    direction = ((p[0x2D] + 0x70) >> 5) & 7;
    switch (direction) {
        case 1:
            Player_SetDirectionalEffectFrame(p + 0x778, *(void **)(p + 0x774),
                                             15 - *(s16 *)(gUnk_02000710 + 0x10));
            Player_SetDirectionalEffectFrame(p + 0x7A0, *(void **)(p + 0x774),
                                             13 - *(s16 *)(gUnk_02000710 + 0x10));
            SetVec(&first, -28, -44, 0);
            SetVec(&second, 12, -4, 0);
            break;
        case 3:
            Player_SetDirectionalEffectFrame(p + 0x778, *(void **)(p + 0x774),
                                             9 - *(s16 *)(gUnk_02000710 + 0x10));
            Player_SetDirectionalEffectFrame(p + 0x7A0, *(void **)(p + 0x774),
                                             15 - *(s16 *)(gUnk_02000710 + 0x10));
            SetVec(&first, 16, -32, 0);
            SetVec(&second, -24, 8, 0);
            break;
        case 5:
            Player_SetDirectionalEffectFrame(p + 0x778, *(void **)(p + 0x774),
                                             11 - *(s16 *)(gUnk_02000710 + 0x10));
            Player_SetDirectionalEffectFrame(p + 0x7A0, *(void **)(p + 0x774),
                                             9 - *(s16 *)(gUnk_02000710 + 0x10));
            SetVec(&first, 16, 8, 0);
            SetVec(&second, -24, -32, 0);
            break;
        case 7:
            Player_SetDirectionalEffectFrame(p + 0x778, *(void **)(p + 0x774),
                                             13 - *(s16 *)(gUnk_02000710 + 0x10));
            Player_SetDirectionalEffectFrame(p + 0x7A0, *(void **)(p + 0x774),
                                             11 - *(s16 *)(gUnk_02000710 + 0x10));
            SetVec(&first, -20, -4, 0);
            SetVec(&second, 20, -44, 0);
            break;
        default:
            *(u32 *)(p + 0x778) |= 1;
            *(u32 *)(p + 0x7A0) |= 1;
            return;
    }
    position.x = *(s16 *)(p + 0x30);
    position.z = *(s16 *)(p + 0x34);
    position.y = *(s16 *)(p + 0x32);
    Project(&projected, &position);
    first.x -= 4;
    first.y -= 4;
    second.x -= 4;
    second.y -= 4;
    Add((struct Vec *)(p + 0x790), &projected, &first);
    Add((struct Vec *)(p + 0x7B8), &projected, &second);
    return;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0813BAD8);
#endif
