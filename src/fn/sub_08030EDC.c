#include "global.h"
#include "ram.h"
/* MessageWindow_SetTextStream: Replaces the primary text stream and clears message table ownership and entry state */
#define MessageWindow_SetTextStream sub_08030EDC

s32 sub_08030EDC(u32 value)
{
    u8 *m = gMessageWindow;
    if (m) {
        u8 *q = m + 0x18;
        *(u32 *)(m + 0x1a4) = 0;
        *(u16 *)(m + 0x196) = 0;
        *(u32 *)(q + 0x1c) = value;
        q[8] = 0;
        q[7] = 0;
        *(u32 *)(m + 0x218) |= 1;
    }
    return 0;
}
