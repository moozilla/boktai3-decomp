#include "global.h"

void SceneActor_ConfigureSecondaryProfileEntry(void *);
void SceneActorControl_TickActionCooldown(void *);

s32 Dog_PrepareFrame(void *p)
{
    SceneActor_ConfigureSecondaryProfileEntry(p);
    SceneActorControl_TickActionCooldown(p);
    return 1;
}
