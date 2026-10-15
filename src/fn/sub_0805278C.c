#include "stacked_actor.h"
#define StackedActor_LowerLift sub_0805278C
void StackedActor_LowerLift(struct StackedActor *actor)
{
    u8 *lift = &actor->lift;
    u32 value = *lift;
    if (value > 1)
        value -= 2;
    else
        value = 0;
    *lift = value;
    actor->sprite.position.y = (u16)actor->world.y + actor->lift;
    if (!actor->lift) {
        actor->sprite.flags &= ~0x200;
        actor->sprite.renderMode = 3;
        actor->shadow.flags |= 1;
        sub_08052628(actor, sub_080527F0);
    }
}
