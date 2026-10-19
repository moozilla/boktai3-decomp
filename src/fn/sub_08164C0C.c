#include "global.h"

#ifdef NONMATCHING
// fuzzy: 88.0%
// foundry draft (worker best 88.0%); what differs is mostly register allocation and instruction order
extern u32 gUnk_030053F4;
u16 *Hud_GetTilemapCell(u32, u32, u32);
void ResultPanel_ClearFiveTiles(void)
{
    if (!(gUnk_030053F4 & 0x1000)) {
        u16 *tiles = Hud_GetTilemapCell(0, 25, 0);
        s32 tile = 0xD000;
        int i = 4;
        do {
            *tiles++ = tile;
        } while (--i >= 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", ResultPanel_ClearFiveTiles);
#endif
