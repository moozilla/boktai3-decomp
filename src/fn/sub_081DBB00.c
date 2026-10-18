// COMPILER: old_agbcc
#include "global.h"
/* ScreenParams2C0_GetSecondaryCount: Returns the second value parsed from the script's n option at parameter offset 0x20 */
#define ScreenParams2C0_GetSecondaryCount sub_081DBB00

extern struct {
    u8 pad[0x20];
    u32 value;
} gUnk_020002C0;

u32 sub_081DBB00(void)
{
    return gUnk_020002C0.value;
}
