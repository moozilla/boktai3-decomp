// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
#define TetherActor_SelectInteractionState sub_080492F0
#define TetherActor_SetState               sub_08049168
#define TetherActor_TestInteractionPath    sub_080492B0
extern struct TetherActor *gUnk_02000488;
void TetherActor_SetState(struct TetherActor *, u32);
u32 TetherActor_TestInteractionPath(struct TetherActor *);
void TetherActor_SelectInteractionState(struct TetherActor *actor)
{
    actor->currentValue = actor->maximumValue;
    asm("" ::: "r3");
    if (!actor->interactionEnabled || actor->effectsSuppressed)
        TetherActor_SetState(gUnk_02000488, 0);
    else if (TetherActor_TestInteractionPath(actor))
        TetherActor_SetState(gUnk_02000488, 3);
    else
        TetherActor_SetState(gUnk_02000488, 4);
}
