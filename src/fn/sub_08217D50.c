#include "global.h"

extern u8 *Palette_GetBackgroundBuffer(void);
extern u8 *gUnk_030051E0;
extern s32 gUnk_030051CC;
extern u16 *gUnk_030042E4;
extern u16 gUnk_030051F0[];
extern u16 gUnk_03004DC0[][16];

void Palette_RefreshObjectBanks(void)
{
    s32 i;
    s32 j;
    s32 next;
    u16 *src;
    u16 *dst;

    gUnk_030051E0 = Palette_GetBackgroundBuffer();
    for (i = 0; i < gUnk_030051CC; i = next) {
        src = (u16 *)((u8 *)gUnk_030042E4 + (gUnk_030051F0[i] << 5));
        dst = gUnk_03004DC0[i];
        next = i + 1;
        j = 15;
        do {
            *dst++ = *src++;
            j--;
        } while (j >= 0);
    }
}
