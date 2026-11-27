// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* SabataRider_ClearManager: Clears the singleton manager pointer. */

void SabataRider_ClearManager(void)
{
    gActorManager = 0;
}
