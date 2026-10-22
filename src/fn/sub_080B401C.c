#include "global.h"

void Vorn_ResumeRouteAfterReaction(void *);
void SceneActor_TickRandomReaction(void *);
void Vorn_TryUphillRoute(void *);
void SceneActorPhase_TryBeginSpecialMotion(void *);
void SceneActor_TrySoloAction(void *);

s32 Vorn_UpdateBehavior(void *p)
{
    Vorn_ResumeRouteAfterReaction(p);
    SceneActor_TickRandomReaction(p);
    Vorn_TryUphillRoute(p);
    SceneActorPhase_TryBeginSpecialMotion(p);
    SceneActor_TrySoloAction(p);
    return 1;
}
