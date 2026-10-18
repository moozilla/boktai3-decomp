// COMPILER: old_agbcc
#include "global.h"
/* PauseMenu_RequestRecoveryRefresh: Clears the accumulated recovery value and sets pending refresh flags; recovery interpretation is tentative */
#define PauseMenu_RequestRecoveryRefresh sub_0816D3E8

void sub_0816D3E8(u8 *p)
{
    *(u16 *)(p + 0x4866) = 0;
    p[0x48e5] = 1;
    p[0x42a0] = 1;
}
