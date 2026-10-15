#include "tether_actor.h"
extern u32 gUnk_02000560;
extern void (*const gUnk_08605A10[])(struct TetherActor *);
void TetherActor_UpdateSegments(struct TetherActor *);
void sub_0815E790(struct TetherPlayer *);
u32 TetherActor_SelectNextState(struct TetherActor *);
void TetherActor_AdjustSlopeMovement(struct TetherActor *);
void TetherActor_UpdateWorldPosition(struct TetherActor *);
void TetherActor_UpdateChargeAndCooldown(struct TetherActor *);
void TetherActor_UpdateEffects(struct TetherActor *);
void TetherActor_UpdateSecondaryColliderValues(struct TetherActor *);
void sub_0821DE30(void *);
static inline u32 segmentsOnly(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
#define TetherActor_Tick sub_0804D378
s32 TetherActor_Tick(struct TetherActor *actor)
{
    actor->blendHold = 0;
    actor->requestStatus = 0;
    if (segmentsOnly()) {
        TetherActor_UpdateSegments(actor);
        return 0;
    }
    if (actor->destroyRequested) {
        Task_Kill(actor);
        return 0;
    }
    if (actor->kind == 9 && actor->requestMode == 0 && actor->onActionScriptId == 0 &&
        gUnk_02000580[0] && gUnk_02000580[0]->unknown1C == 1 &&
        gUnk_02000580[0]->tetherMode != 24) {
        sub_0815E790(gUnk_02000580[0]);
        gUnk_02000580[0]->tetherMode = 36;
        gUnk_02000580[0]->unknown458 = 0;
        gUnk_02000580[0]->unknown45A = 0;
    }
    actor->shakeX = 0;
    actor->shakeZ = 0;
    actor->moving = 0;
    {
        u32 state = TetherActor_SelectNextState(actor);
        sub_0824923C(actor, gUnk_08605A10[state]);
    }
    TetherActor_AdjustSlopeMovement(actor);
    TetherActor_UpdateWorldPosition(actor);
    TetherActor_UpdateChargeAndCooldown(actor);
    TetherActor_UpdateEffects(actor);
    TetherActor_UpdateSecondaryColliderValues(actor);
    actor->interactionProbe.position = &actor->position;
    sub_0821DE30(&actor->interactionProbe.bodyId);
    if (actor->destroyRequested)
        Task_Kill(actor);
    return 0;
}
