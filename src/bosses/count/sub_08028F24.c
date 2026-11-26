#include "global.h"

void Count_UpdateFrame(void *);
void SceneActorState_UpdateActionController(void *);
void SceneActors_RunUnpausedState(void *);
void Count_UpdatePresentation(void *);

s32 Count_Init(void *p)
{
    Count_UpdateFrame(p);
    SceneActorState_UpdateActionController(p);
    SceneActors_RunUnpausedState(p);
    Count_UpdatePresentation(p);
    return 0;
}
