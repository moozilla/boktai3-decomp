#include "scene/actor/scene_actor_reaction.h"
void SceneActorReaction_BeginFadedState(struct ChaseActor *actor)
{
    u32 state;
    struct SceneActorReactionGroup *group = (void *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorReaction_ResetRouteControl(actor, &group->control);
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 3, 1);
        switch (actor->base.category) {
            case 4:
                Sound_PlaySong(212);
                break;
            case 1:
                Sound_PlaySong(253);
                break;
            case 3:
                SceneActorPlacement_PlayVariantEventSound((void *)actor, 1);
                break;
        }
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            u32 old;
            old = collider->flags;
            collider->flags = 4 | collider->flags;
            actor->base.colliderA.responseValue = 0;
        }
        if (actor->base.drawableKind == 0) {
            u32 *sequence = actor->base.drawable;
            u32 mask = ~1;
            *sequence &= mask;
        } else {
            u8 *tile = (u8 *)actor->base.drawable + 0x20;
            u32 mask = ~1;
            *(u32 *)(tile + 8) &= mask;
        }
        {
            u32 zero;
            u8 *first;
            state = 3;
            first = &actor->base.state2A8;
            zero = 0;
            *first = state;
            actor->base.phase2A9 = zero;
            actor->base.behavior2AA = zero;
            actor->base.counter2AC = zero;
            actor->base.field2B1 = 1;
        }
        actor->base.attachmentTimer = 0;
        if (actor->base.drawableKind == 0)
            ((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sprite.priority = state;
        else
            ((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile.attribute1A = state;
        {
            u32 mask = 0x80000;
            u32 *field;
            field = &actor->base.statusFlags;
            *field |= mask;
        }
        ActorShadow_Disable(actor->base.shadow);
    }
}
