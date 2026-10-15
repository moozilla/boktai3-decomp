// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#define SceneActorReaction_BeginFadedState sub_08080868
void SceneActorReaction_BeginFadedState(struct SceneActorRegistryResetView *actor)
{
    u32 state;
    struct SceneActorReactionGroup *group = (void *)&actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorRegistryControl *control = &group->control;
        u32 *field = &actor->flags2CC;
        u32 mask = ~0x141C0;
        *field &= mask;
        control->counter = 0;
        *(u16 *)((u8 *)control + 0x14) = 0;
        control->timer = 0;
        sub_080764C8(actor, 3, 1);
        switch (actor->category) {
            case 4:
                Sound_PlaySong(212);
                break;
            case 1:
                Sound_PlaySong(253);
                break;
            case 3:
                sub_08077848(actor, 1);
                break;
        }
        {
            struct ContactCollider *collider = &actor->colliderA;
            register u32 old asm("r1");
            register u32 flag asm("r0");
            u32 zero;
            old = collider->flags;
            flag = 4;
            zero = 0;
            collider->flags = flag | old;
            *(u16 *)((u8 *)actor + 0x178) = zero;
        }
        if (actor->drawableKind == 0) {
            u32 *sequence = actor->drawable;
            u32 mask = ~1;
            *sequence &= mask;
        } else {
            register u8 *tile asm("r0") = (u8 *)actor->drawable + 0x20;
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
            register u32 mask asm("r2") = 0x80000;
            register u32 *field asm("r1");
            field = &actor->flags2D0;
            *field |= mask;
        }
        ActorShadow_Disable((u8 *)actor + 0x320);
    }
}
