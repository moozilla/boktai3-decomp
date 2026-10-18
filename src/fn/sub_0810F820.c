// COMPILER: old_agbcc
#include "global.h"
/* LinkLobbyPanel_ResetInstance: Clears the panel singleton pointer */
#define LinkLobbyPanel_ResetInstance sub_0810F820

extern u32 gUnk_020001D0;

void sub_0810F820(void)
{
    gUnk_020001D0 = 0;
}
