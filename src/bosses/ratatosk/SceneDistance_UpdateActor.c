#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void SceneActors_UpdateTargetMetrics(void *);
void SceneActors_UpdateEffects(void *);
struct ContactPosition *Ratatosk_GetRandomMovementPoint(void *);
void Ratatosk_UpdateAnimatedPosition(void *);
void Ratatosk_UpdateBehavior(void *);
u32 Ratatosk_InitRequestedState(void *);
void Ratatosk_RunStateCallback(void *);
void Ratatosk_UpdateCollidersAndPalette(void *);
s32 SceneDistance_UpdateActor(struct Ratatosk *p)
{
    u8 previous = p->nearTarget;
    SceneActors_UpdateTargetMetrics(p);
    if (previous != p->nearTarget) {
        p->destination = *Ratatosk_GetRandomMovementPoint(p);
        p->savedState = 2;
    }
    Ratatosk_UpdateAnimatedPosition(p);
    Ratatosk_UpdateBehavior(p);
    Ratatosk_InitRequestedState(p);
    Ratatosk_RunStateCallback(p);
    SceneActors_UpdateEffects(p);
    Ratatosk_UpdateCollidersAndPalette(p);
    return 0;
}
