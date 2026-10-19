#include "global.h"

void sub_080B3464(void *);
void SceneActor_TickRandomReaction(void *);
void sub_080B162C(void *);
void SceneActorPhase_TryBeginSpecialMotion(void *);
void SceneActor_TrySoloAction(void *);

s32 Vorn_UpdateBehavior(void *p)
{
    sub_080B3464(p);
    SceneActor_TickRandomReaction(p);
    sub_080B162C(p);
    SceneActorPhase_TryBeginSpecialMotion(p);
    SceneActor_TrySoloAction(p);
    return 1;
}
