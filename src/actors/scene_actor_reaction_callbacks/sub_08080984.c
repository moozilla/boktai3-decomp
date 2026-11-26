#include "actors/scene_actor_reaction/scene_actor_reaction.h"
void SceneActorReaction_BeginReactionSound(struct ChaseActor *actor)
{
    u32 state;
    u32 timerZero;
    struct SceneActorReactionGroup *group = (void *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorRegistryControl *control = &group->control;
        u32 *field = &actor->base.behaviorFlags;
        u32 mask = ~0x141C0;
        *field &= mask;
        control->counter = 0;
        control->field14 = 0;
        control->timer = 0;
        {
            switch (actor->base.category) {
                case 4:
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
                    Sound_PlaySong(212);
                    break;
                case 1:
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
                    Sound_PlaySong(253);
                    break;
                case 3:
                case 5:
                case 6:
                case 7:
                case 21:
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
                    Sound_PlaySong(318);
                    break;
                case 25:
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
                    Sound_PlaySong(459);
                    break;
            }
            {
                u16 *timer = (u16 *)(((u8 *)actor) + 0x178);
                timerZero = 0;
                *timer = timerZero;
            }
        }
        while (0)
            ;
        if (actor->base.drawableKind == 0) {
            u32 *sequence = actor->base.drawable;
            u32 mask = timerZero - 2;
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
    }
}
