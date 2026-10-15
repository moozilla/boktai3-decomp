// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#define SceneActorReaction_BeginReactionSound sub_08080984
void SceneActorReaction_BeginReactionSound(struct SceneActorRegistryResetView *actor)
{
    u32 state;
    u32 timerZero;
    struct SceneActorReactionGroup *group = (void *)&actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorRegistryControl *control = &group->control;
        u32 *field = &actor->flags2CC;
        u32 mask = ~0x141C0;
        *field &= mask;
        control->counter = 0;
        *(u16 *)((u8 *)control + 0x14) = 0;
        control->timer = 0;
        switch (actor->category) {
            case 4:
                sub_08076034(actor, 3);
                Sound_PlaySong(212);
                break;
            case 1:
                sub_08076034(actor, 3);
                Sound_PlaySong(253);
                break;
            case 3:
            case 5:
            case 6:
            case 7:
            case 21:
                sub_08076034(actor, 3);
                Sound_PlaySong(318);
                break;
            case 25:
                sub_08076034(actor, 3);
                Sound_PlaySong(459);
                break;
        }
        {
            u16 *timer = (u16 *)((u8 *)actor + 0x178);
            timerZero = 0;
            asm("" : "+r"(timerZero));
            *timer = timerZero;
        }
        if (actor->drawableKind == 0) {
            u32 *sequence = actor->drawable;
            u32 mask = timerZero - 2;
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
    }
}
