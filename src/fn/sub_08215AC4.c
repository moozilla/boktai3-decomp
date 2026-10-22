#include "global.h"

#ifdef NONMATCHING
// fuzzy: 97.6%
// ROM loads BGxCNT before the 0xFFF3 mask but ANDs into the mask register; this
// version gets the registers right with the mask materialised first.
// Sets the character-base block (BGxCNT bits 2-3) of background `bg`.
void sub_08215AC4(s32 bg, u32 charBase)
{
    vu16 *cnt;

    switch (bg) {
        case 0:
            cnt = (vu16 *)0x04000008;
            break;
        case 1:
            cnt = (vu16 *)0x0400000A;
            break;
        case 2:
            cnt = (vu16 *)0x0400000C;
            break;
        case 3:
            cnt = (vu16 *)0x0400000E;
            break;
        default:
            return;
    }
    {
        u32 m = 0xFFF3;
        u16 v = (m & *cnt) | (charBase << 2);
        *cnt = v;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08215AC4);
#endif
