#include "engine/shared/global.h"

void SceneActors_RunStateCallback(void *);

void SceneActors_TickStateCallback(void *actor)
{
    SceneActors_RunStateCallback(actor);
}
