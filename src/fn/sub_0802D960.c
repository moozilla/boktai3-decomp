#include "global.h"
/* Actor_RestoreDefaultPaletteBank: Reapplies the saved default palette bank to the drawable */
#define Actor_RestoreDefaultPaletteBank sub_0802D960

s32 Actor_SetPaletteBank(u8 *, u16);

s32 sub_0802D960(u8 *p)
{
    return Actor_SetPaletteBank(p, *(u16 *)(p + 0x1c));
}
