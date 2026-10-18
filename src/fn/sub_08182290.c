#include "global.h"

void Callback_InvokeActor(u8 *, u32);

void Armor_UpdatePaletteRestoreDelay(u8 *p)
{
    s32 c = *(u16 *)(p + 0x488) + 1;
    u32 z = 0;
    *(u16 *)(p + 0x488) = c;
    if ((s16)c > 7) {
        u32 q;
        *(u32 *)p = z;
        q = *(u32 *)(p + 4);
        if (q) {
            Callback_InvokeActor(p, q);
            *(u32 *)(p + 4) = z;
        }
    }
}
