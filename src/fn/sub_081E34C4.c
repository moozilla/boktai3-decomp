// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* Actor_ClearSingletonInstance: Clears the global instance pointer used by the singleton scripted actor controller; ownership is provisional */
#define Actor_ClearSingletonInstance sub_081E34C4

void sub_081E34C4(void)
{
    gScriptedActorController = 0;
}
