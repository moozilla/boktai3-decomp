#include "global.h"

void Vampire_UpdateFrame(void);
void SceneActorState_UpdateActionController(void *);
void SceneActors_RunUnpausedState(void *);
void sub_080289FC(void *);

s32 Vampire_Init(void *p)
{
    Vampire_UpdateFrame();
    SceneActorState_UpdateActionController(p);
    SceneActors_RunUnpausedState(p);
    sub_080289FC(p);
    return 0;
}
