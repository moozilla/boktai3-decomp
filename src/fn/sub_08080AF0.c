#include "scene_actor_reaction.h"
void SceneActorReaction_PlayAnimationBUntilComplete(void *);
static inline u8 TakePrepared(struct ChaseActor *actor)
{
    u8 *field;
    field = &actor->base.field2B2;
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}
static inline void SceneActorReaction_UpdateReturnToRoute(struct ChaseActor *actor,
                                                          struct SceneActorReactionGroup *group)
{
    u32 mask;
    if (TakePrepared(actor)) {
        actor->base.body.direction = (((actor->base.body.direction + 128) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    }
    mask = 0x10;
    if (actor->base.statusFlags & mask) {
        struct SceneActorReactionCommand *command;
        void *callback;
        u8 one;
        u32 zero;
        u8 kind;
        actor->base.behaviorFlags |= mask;
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
        actor->base.field2B2 = one;
        actor->base.stepDone = zero;
        actor->base.controlKind = kind;
        actor->base.target = callback;
        {
            u8 byteZero;
            u16 *halfField;
            u32 *field;
            u32 clearMask;
            u32 state;
            halfField = &actor->base.field2C8;
            byteZero = 0;
            *halfField = zero;
            field = &actor->base.pendingControlFlags;
            clearMask = ~1;
            *field &= clearMask;
            actor->base.pendingControl = zero;
            state = 3;
            actor->base.state2A8 = state;
            actor->base.phase2A9 = one;
            actor->base.behavior2AA = byteZero;
            {
                actor->base.counter2AC = zero;
            }
            actor->base.field2B1 = one;
            {
                u16 *cooldown = &actor->base.field2C2;
                *cooldown = 64;
            }
        }
    }
}

void SceneActorReaction_TurnBackToRoute(struct ChaseActor *actor)
{
    SceneActorReaction_UpdateReturnToRoute(actor, (void *)&actor->group.common);
}
