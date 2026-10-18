#include "global.h"
#include "state_actor.h"

void GroundEffect_EnableProjection(void *);
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
s32 Player_GetHorizontalVelocity(s32);
void SceneNpc_UpdateProjectedIdle(void *unused, struct StateActor *p)
{
    if (p->enter) {
        p->enter = 0;
        p->done = 0;
        GroundEffect_EnableProjection(p->groundEffect);
        Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
    }
    p->counter300 += Player_GetHorizontalVelocity(0);
    p->animationCallback(p);
}
