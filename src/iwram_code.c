/* ARM C copied to IWRAM 0x03000000 by AgbMain (ROM 08000350-0800145C). */
// COMPILER: agbcc_arm
// CFLAGS: -O2 -mthumb-interwork
#include "global.h"

struct PaletteBank {
    u32 words[8];
};

extern s32 gUnk_03004DA4;
extern u16 gUnk_030051F0[];
extern struct PaletteBank gUnk_03004DC0[];

/* Same cache as Palette_CacheObjectBank, with a word copy instead of CpuFastSet. */
u32 Palette_CacheObjectBankArm(u32 key, u32 *src)
{
    s32 i = 0;
    for (; i < gUnk_03004DA4; i++)
        if (gUnk_030051F0[i] == key)
            goto found;
    if (gUnk_03004DA4 > 15)
        goto overflow;
    {
        s32 n = gUnk_03004DA4;
        u32 *dst = gUnk_03004DC0[n].words;
        dst[0] = src[0];
        gUnk_030051F0[n] = key;
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        return gUnk_03004DA4++;
    }
found:
    return i;
overflow:
    return 0;
}

struct IwVec {
    s16 x, y, z;
};

struct IwObj {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x14];
    struct IwVec pos;
    u16 dx;
    u16 dy;
    s16 right;
    s16 bottom;
    s16 left;
    s16 top;
    u8 pad34[0x28];
    struct IwObj *next;
};

extern u32 gUnk_03003A50;
extern u32 gUnk_03004290;
extern struct IwObj *gUnk_030042C8[];
extern u16 gUnk_03005418[];
void sub_08000430(struct IwObj *, s32, s32, s32);

static inline s32 IwProjectScale(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}

static inline void IwProject(struct IwVec *screen, struct IwVec *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum, vertical, scaledSum;
    screen->x = IwProjectScale(3 * (x - z));
    scaledSum = IwProjectScale(3 * (x + z));
    sum = scaledSum;
    vertical = IwProjectScale(3 * world->y);
    screen->x = (u16)screen->x - gUnk_03005418[0] + 120;
    screen->y = sum - vertical - gUnk_03005418[1] + 80;
    screen->z = sum + vertical - gUnk_03005418[2];
}

INCLUDE_ASM("asm/nonmatching", sub_08000430);
void sub_08000E90(void)
{
    struct IwObj *o;
    u32 mask;
    s32 x, y, z;
    struct IwVec s;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        if (!(o->flags & 0x10)) {
            IwProject(&s, &o->pos);
            x = s.x + o->dx;
            y = s.y + o->dy;
            z = s.z;
        } else {
            x = o->pos.x + o->dx;
            y = o->pos.y + o->dy;
            z = o->pos.z;
        }
        if (o->flags & 0x40) {
            if (x + o->left < 0)
                continue;
            if (y + o->top < 0)
                continue;
            if (x + o->right > 0xF0)
                continue;
            if (y + o->bottom > 0xA0)
                continue;
        }
        sub_08000430(o, x, y, z);
    }
}

void sub_08001038(void)
{
    struct IwObj *o;
    u32 mask;
    s32 x, y, z;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        x = (s16)(o->pos.x + o->dx);
        y = (s16)(o->pos.y + o->dy);
        z = o->pos.z;
        if (!(o->flags & 0x40)) {
            if (x + o->left < 0)
                continue;
            if (y + o->top < 0)
                continue;
            if (x + o->right > 0xF0)
                continue;
            if (y + o->bottom > 0xA0)
                continue;
        }
        sub_08000430(o, x, y, z);
    }
}

#ifdef NONMATCHING
// fuzzy: 93.8%
// Left: ROM copies flags (mov r5, r3) before the mask test's branch; flags and z then swap r5/lr.
void sub_08001120(void)
{
    struct IwObj *o;
    u32 mask, flags;
    s32 x, y, z;
    struct IwVec s;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        flags = o->flags;
        if (!(flags & 0x1000))
            continue;
        if (!(flags & 0x10)) {
            IwProject(&s, &o->pos);
            x = (s16)(s.x + o->dx);
            y = (s16)(s.y + o->dy);
            z = s.z;
        } else {
            x = (s16)(o->pos.x + o->dx);
            y = (s16)(o->pos.y + o->dy);
            z = o->pos.z;
        }
        if (!(flags & 0x40)) {
            if (x + o->left < 0)
                continue;
            if (y + o->top < 0)
                continue;
            if (x + o->right > 0xF0)
                continue;
            if (y + o->bottom > 0xA0)
                continue;
        }
        sub_08000430(o, x, y, z);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08001120);
#endif

void sub_080012E0(void)
{
    struct IwObj *o;
    u32 mask;
    s32 x, y, z;
    struct IwVec s;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        if (!(o->flags & 0x2000))
            continue;
        if (!(o->flags & 0x10)) {
            IwProject(&s, &o->pos);
            x = (s16)(s.x + o->dx);
            y = (s16)(s.y + o->dy);
            z = s.z;
        } else {
            x = (s16)(o->pos.x + o->dx);
            y = (s16)(o->pos.y + o->dy);
            z = o->pos.z;
        }
        sub_08000430(o, x, y, z);
    }
}
