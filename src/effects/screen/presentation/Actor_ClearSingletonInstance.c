// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* Actor_ClearSingletonInstance: Clears the global instance pointer used by the singleton scripted actor controller; ownership is provisional */

void Actor_ClearSingletonInstance(void)
{
    gScriptedActorController = 0;
}
