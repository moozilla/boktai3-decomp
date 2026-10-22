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

INCLUDE_ASM("asm/nonmatching", sub_08000430);
INCLUDE_ASM("asm/nonmatching", sub_08000E90);
struct IwObj {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x14];
    u16 x;
    u16 y;
    s16 z;
    u16 pad26;
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
void sub_08000430(struct IwObj *, s32, s32, s32);

void sub_08001038(void)
{
    struct IwObj *o;
    u32 mask;
    s32 x, y, z;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        x = (s16)(o->x + o->dx);
        y = (s16)(o->y + o->dy);
        z = o->z;
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

INCLUDE_ASM("asm/nonmatching", sub_08001120);
INCLUDE_ASM("asm/nonmatching", sub_080012E0);
