// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ActorMarker_ResetManager: clears the singleton pointer for the two-slot actor marker manager; marker identity is tentative */
#define ActorMarker_ResetManager sub_0813A958

void sub_0813A958(void)
{
    gCompactSpritePool = 0;
}
