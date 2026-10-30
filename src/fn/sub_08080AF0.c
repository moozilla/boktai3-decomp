// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
void SceneActorReaction_PlayAnimationBUntilComplete(void *);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *actor)
{
    register u32 offset asm("r2") = 0x2B2;
    u8 *field;
    asm("" : "+r"(offset));
    field = (u8 *)actor + offset;
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}
void SceneActorReaction_TurnBackToRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionGroup *group = (void *)&actor->group;
    u32 offset;
    u32 mask;
    if (TakePrepared(actor)) {
        actor->body.direction = (((actor->body.direction + 128) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    }
    mask = 0x10;
    offset = 0x2D0;
    asm("" : "+r"(offset));
    if (*(u32 *)((u8 *)actor + offset) & mask) {
        struct SceneActorReactionCommand *command;
        register void *callback asm("r2");
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
        /* Keep the related prepared2B2/substate2B0/kind2B5 offsets together. */
        offset -= 0x1E;
        *((u8 *)actor + offset) = one;
        offset -= 2;
        *((u8 *)actor + offset) = zero;
        offset += 5;
        *((u8 *)actor + offset) = kind;
        actor->target = callback;
        {
            register u32 fieldOffset asm("r2") = 0x2C8;
            u8 byteZero;
            register u16 *halfField asm("r0");
            u32 *field;
            u32 clearMask;
            u32 animationOffset;
            u32 state;
            asm("" : "+r"(fieldOffset));
            halfField = (void *)((u8 *)actor + fieldOffset);
            byteZero = 0;
            *halfField = zero;
            field = &actor->flags114;
            clearMask = ~1;
            *field &= clearMask;
            actor->word104 = zero;
            /* State2A8, phase2A9, behavior2AA, counter2AC, cooldown2C2. */
            state = 3;
            animationOffset = 0x2A8;
            *((u8 *)actor + animationOffset) = state;
            *((u8 *)actor + 0x2A9) = one;
            animationOffset += 2;
            *((u8 *)actor + animationOffset) = byteZero;
            {
                register u32 counterOffset asm("r5") = 0x2AC;
                *(u32 *)((u8 *)actor + counterOffset) = zero;
                asm("" : : "r"(counterOffset));
            }
            actor->field2B1 = one;
            animationOffset += 0x18;
            {
                register u16 *cooldown asm("r1") = (void *)((u8 *)actor + animationOffset);
                *cooldown = 64;
                asm("" : : "r"(animationOffset));
            }
        }
    }
}
