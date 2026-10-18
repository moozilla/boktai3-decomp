#include "global.h"

void sub_080287DC(void);
void SceneActorState_UpdateActionController(void *);
void SceneActors_RunUnpausedState(void *);
void sub_080289FC(void *);

s32 Vampire_Init(void *p)
{
    sub_080287DC();
    SceneActorState_UpdateActionController(p);
    SceneActors_RunUnpausedState(p);
    sub_080289FC(p);
    return 0;
}
