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
INCLUDE_ASM("asm/nonmatching", sub_08001038);
INCLUDE_ASM("asm/nonmatching", sub_08001120);
INCLUDE_ASM("asm/nonmatching", sub_080012E0);
