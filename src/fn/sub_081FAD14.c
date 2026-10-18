#define SceneDistance_UpdateActor          sub_081FAD14
#define SceneDistance_SelectRandomPosition Ratatosk_GetRandomMovementPoint
#define SceneDistance_ApplyRequestedState Ratatosk_InitRequestedState
#define SceneDistance_RunStateCallback Ratatosk_RunStateCallback
#define SceneDistance_DispatchState Ratatosk_UpdateBehavior
#include "global.h"
#include "scene_vec.h"
struct SceneDistanceActor {
    u8 pad0[0xA0];
    u8 nearTarget;
    u8 padA1[0xE5C - 0xA1];
    struct SceneVec selectedPosition;
    u8 padE64[0xEF0 - 0xE64];
    u32 phase;
};
void SceneActors_UpdateTargetMetrics(void *);
void SceneActors_UpdateEffects(void *);
struct SceneVec *Ratatosk_GetRandomMovementPoint(void *);
void sub_081FA638(void *);
void Ratatosk_UpdateBehavior(void *);
u32 Ratatosk_InitRequestedState(void *);
void Ratatosk_RunStateCallback(void *);
void sub_081FA718(void *);
s32 sub_081FAD14(struct SceneDistanceActor *p)
{
    u8 previous = p->nearTarget;
    SceneActors_UpdateTargetMetrics(p);
    if (previous != p->nearTarget) {
        p->selectedPosition = *Ratatosk_GetRandomMovementPoint(p);
        p->phase = 2;
    }
    sub_081FA638(p);
    Ratatosk_UpdateBehavior(p);
    Ratatosk_InitRequestedState(p);
    Ratatosk_RunStateCallback(p);
    SceneActors_UpdateEffects(p);
    sub_081FA718(p);
    return 0;
}
