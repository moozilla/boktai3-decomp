#include "global.h"

void SceneActors_RunStateCallback(void *);

void Ratatosk_RunStateCallback(void *actor)
{
    SceneActors_RunStateCallback(actor);
}
