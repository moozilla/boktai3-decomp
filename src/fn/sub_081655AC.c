#include "global.h"

#ifdef NONMATCHING
// fuzzy: 90.0%
// foundry draft (worker best 90.0%); what differs is mostly register allocation and instruction order
u16 *Hud_GetTilemapCell(u32, u32, u32);
void ResultPanel_FillTileRun(s32 count, u32 x, u32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    if (count > 0) {
        s32 i = count;
        do {
            *p++ = 0xF090;
        } while (--i);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", ResultPanel_FillTileRun);
#endif
