#include "global.h"

void SceneActors_RunStateCallback(void *);

void VanargandHead_TickStateCallback(void *actor)
{
    SceneActors_RunStateCallback(actor);
}
