// COMPILER: old_agbcc
#include "global.h"
/* Actor_ClearSingletonInstance: Clears the global instance pointer used by the singleton scripted actor controller; ownership is provisional */
#define Actor_ClearSingletonInstance sub_081E34C4

extern u32 gUnk_020003C8;

void sub_081E34C4(void)
{
    gUnk_020003C8 = 0;
}
