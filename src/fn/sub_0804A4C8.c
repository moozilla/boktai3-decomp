// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
extern const u8 gUnk_08605A04[];
void TetherActor_SelectInteractionState(struct TetherActor *);
void TetherActor_InitInteractionFromScript(struct TetherActor *actor)
{
    s32 value = Script_SeekToKeyword('r');
    register u32 capacity asm("r0");
    if (value) {
        actor->interactionTarget.x = Script_GetValue();
        actor->interactionTarget.y = Script_GetValue();
        actor->interactionTarget.z = Script_GetValue();
    } else {
        actor->interactionTarget.x = value;
        actor->interactionTarget.y = value;
        actor->interactionTarget.z = value;
    }
    value = Script_SeekToKeyword('O');
    if (value)
        actor->interactionValue = Script_GetValue();
    else
        actor->interactionValue = value;
    capacity = gUnk_08605A04[actor->kind] * 120;
    actor->maximumValue = capacity;
    actor->currentValue = capacity;
    if (actor->kind == 2 || actor->kind == 9) {
        actor->interactionEnabled = 0;
        if ((u8)(actor->state - 3) <= 1)
            actor->state = 0;
    } else {
        actor->interactionEnabled = 1;
        if ((u8)(actor->state - 3) <= 1)
            TetherActor_SelectInteractionState(actor);
    }
    capacity = gUnk_08605A04[actor->kind] * 60;
    actor->secondaryMaximum = capacity;
    actor->secondaryValue = capacity;
    actor->secondaryTimer = 300;
    actor->secondaryMode = 0;
}
