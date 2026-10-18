// COMPILER: old_agbcc
#include "global.h"
/* ActorMarker_ResetManager: clears the singleton pointer for the two-slot actor marker manager; marker identity is tentative */
#define ActorMarker_ResetManager sub_0813A958

extern u32 gUnk_020004D8;

void sub_0813A958(void)
{
    gUnk_020004D8 = 0;
}
