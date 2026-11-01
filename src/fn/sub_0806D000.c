#include "route_sprite_playback.h"
void RouteSpriteActor_FinishAnimation(struct RouteSpritePool *pool, struct RouteSpriteActor *actor)
{
    if (actor->entered) {
        Anim_SetSequence(&actor->sequence, pool->sequenceResource, 0x17, 0, 4);
        actor->entered = 0;
    }
    if (RouteSpriteActor_StepPlayback(actor)) {
        u32 offset = 0x153;
        s8 *indexPointer;
        indexPointer = (s8 *)((u32)actor + offset);
        pool->occupiedMask &= ~(1 << *indexPointer);
        actor->state = 0;
        if (actor->sprite.unknown4[0])
            Sprite_Remove(&actor->sprite);
        if (actor->shadowAbsent == 0)
            ScaledShadow_Destroy(actor->shadow);
        *(u8 *)indexPointer = 0xFF;
        actor->flags150 = 0;
    }
}
