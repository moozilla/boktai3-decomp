#include "global.h"

void Vampire_UpdateFrame(void *);
void SceneActorState_UpdateActionController(void *);
void SceneActors_RunUnpausedState(void *);
void Vampire_UpdatePresentation(void *);

s32 Vampire_Init(void *p)
{
    Vampire_UpdateFrame(p);
    SceneActorState_UpdateActionController(p);
    SceneActors_RunUnpausedState(p);
    Vampire_UpdatePresentation(p);
    return 0;
}
