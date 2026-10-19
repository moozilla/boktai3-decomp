#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.0%
// foundry draft (worker best 87.0%); what differs is mostly register allocation and instruction order
#define StatusPanel_ClearLowerRows sub_08169BB0
#define Tilemap_GetCell            Hud_GetTilemapCell
u16 *Hud_GetTilemapCell(s32, u32, u32);
void sub_08169BB0(void)
{
    s32 row = 0;
    u16 blank = 0xD000;
    do {
        u16 *p = Hud_GetTilemapCell(0, 9, row + 17);
        s32 next = row + 1;
        s32 col;
        col = 11;
        do {
            *p++ = (u16)(blank | 0);
        } while (--col >= 0);
        row = next;
    } while (row <= 1);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08169BB0);
#endif
