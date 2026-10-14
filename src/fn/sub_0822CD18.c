#include "global.h"
extern u8 *gUnk_02000710;
/* Store the low 15 bits of game-state word 0x100 + 2 * index, keeping bit 15. */
void sub_0822CD18(s32 index, u32 value)
{
    ((s16 *)(gUnk_02000710 + 0x100))[index] =
        (value & 0x7fff) | (((s16 *)(gUnk_02000710 + 0x100))[index] & 0x8000);
}
