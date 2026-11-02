#include "scene_actor_reaction.h"
void SceneActorReaction_PlayAnimationBUntilComplete(void *);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *actor)
{
    u8 *field;
    field = &actor->field2B2;
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}
static inline void SceneActorReaction_UpdateReturnToRoute(struct SceneActorRegistryResetView *actor,
                                                          struct SceneActorReactionGroup *group)
{
    u32 mask;
    if (TakePrepared(actor)) {
        actor->body.direction = (((actor->body.direction + 128) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    }
    mask = 0x10;
    if (actor->flags2D0 & mask) {
        struct SceneActorReactionCommand *command;
        void *callback;
        u8 one;
        u32 zero;
        u8 kind;
        actor->flags2CC |= mask;
        command = (void *)((u8 *)group + 0x8C);
        callback = (void *)SceneActorReaction_PlayAnimationBUntilComplete;
        one = 1;
        command->duration = 30;
        command->payload = callback;
        zero = 0;
        command->first = zero;
        command->second = one;
        command->kind = one;
        callback = (void *)SceneActorReaction_RestoreAfterDelay;
        kind = 14;
        actor->field2B2 = one;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        {
            u8 byteZero;
            u16 *halfField;
            u32 *field;
            u32 clearMask;
            u32 state;
            halfField = &actor->field2C8;
            byteZero = 0;
            *halfField = zero;
            field = &actor->flags114;
            clearMask = ~1;
            *field &= clearMask;
            actor->word104 = zero;
            state = 3;
            actor->state2A8 = state;
            actor->phase2A9 = one;
            actor->behavior2AA = byteZero;
            {
                actor->counter2AC = zero;
            }
            actor->field2B1 = one;
            {
                u16 *cooldown = &actor->field2C2;
                *cooldown = 64;
            }
        }
    }
}

void SceneActorReaction_TurnBackToRoute(struct SceneActorRegistryResetView *actor)
{
    SceneActorReaction_UpdateReturnToRoute(actor, (void *)&actor->group.common);
}
