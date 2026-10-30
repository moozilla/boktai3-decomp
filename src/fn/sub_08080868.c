#include "scene_actor_reaction.h"
void SceneActorReaction_BeginFadedState(struct SceneActorRegistryResetView *actor)
{
    u32 state;
    struct SceneActorReactionGroup *group = (void *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorRegistryControl *control = &group->control;
        u32 *field = &actor->flags2CC;
        u32 mask = ~0x141C0;
        *field &= mask;
        control->counter = 0;
        *(u16 *)((u8 *)control + 0x14) = 0;
        control->timer = 0;
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 3, 1);
        switch (actor->category) {
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
            struct ContactCollider *collider = &actor->colliderA;
            u32 old;
            u32 flag;
            u32 zero;
            old = collider->flags;
            flag = 4;
            collider->flags = flag | collider->flags;
            zero = 0;
            *(u16 *)((u8 *)actor + 0x178) = zero;
        }
        if (actor->drawableKind == 0) {
            u32 *sequence = actor->drawable;
            u32 mask = ~1;
            *sequence &= mask;
        } else {
            u8 *tile = (u8 *)actor->drawable + 0x20;
            u32 mask = ~1;
            *(u32 *)(tile + 8) &= mask;
        }
        {
            u32 zero;
            u8 *first;
            state = 3;
            first = (u8 *)actor + 0x2A8;
            zero = 0;
            *first = state;
            *((u8 *)actor + 0x2A9) = zero;
            *((u8 *)actor + 0x2AA) = zero;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
        }
        actor->fieldAA = 0;
        if (actor->drawableKind == 0)
            *((u8 *)actor->drawable + 7) = state;
        else
            *((u8 *)actor->drawable + 0x3A) = state;
        {
            u32 mask = 0x80000;
            u32 *field;
            field = &actor->flags2D0;
            *field |= mask;
        }
        ActorShadow_Disable((u8 *)actor + 0x320);
    }
}
