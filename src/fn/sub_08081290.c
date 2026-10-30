// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void SceneActorReaction_BeginHiddenDefeat(struct SceneActorRegistryResetView *);
static inline u8 IsControlled(struct SceneActorRegistryResetView *a)
{
    if (a->object == (void *)1)
        return 1;
    return 0;
}
static inline void ClearRoute(struct SceneActorRegistryResetView *a)
{
    register struct SceneActorRegistryControl *route asm("r2") = &a->group.control;
    register u32 *flags asm("r3") = &a->flags2CC;
    register u32 mask asm("r1") = ~0x141C0;
    *flags &= mask;
    route->counter = 0;
    *(u16 *)((u8 *)route + 0x14) = 0;
    route->timer = 0;
}
static inline void Show(struct SceneActorRegistryResetView *a)
{
    if (!a->drawableKind) {
        u32 *flags = a->drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)a->drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}
void SceneActorReaction_SinkAndRemove(struct SceneActorRegistryResetView *input, s32 frame)
{
    struct SceneActorRegistryResetView *actor = input;
    asm("" : "+r"(actor));
    if (SceneActorReaction_TakePrepared(actor)) {
        struct ContactCollider *collider = &actor->colliderA;
        u32 category = actor->category;
        if (IsControlled(actor))
            ClearRoute(actor);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        if (category != 2 && category != 22) {
            register u32 forwarded asm("r3") = category;
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, forwarded);
        }
        {
            register u32 value asm("r0") = collider->flags;
            u32 mask = 4;
            collider->flags = mask | value;
        }
        Show(actor);
        {
            register u32 state asm("r1") = 3;
            u8 *field = &actor->state2A8;
            u32 zero = 0;
            *field = state;
            {
                register u32 phaseOffset asm("r3") = 0x2A9;
                asm("" : : "r"(phaseOffset));
                field = (u8 *)actor + phaseOffset;
            }
            *field = zero;
            {
                register u32 behaviorOffset asm("r1") = 0x2AA;
                asm("" : : "r"(behaviorOffset));
                field = (u8 *)actor + behaviorOffset;
            }
            *field = zero;
            {
                u32 *counter = &actor->counter2AC;
                *counter = zero;
            }
            {
                u32 readyOffset = 0x2B1;
                u8 *ready;
                ready = (u8 *)actor + readyOffset;
                *ready = 1;
            }
            {
                u32 byteZero = 0;
                actor->fieldAA = zero;
                actor->field2B3 = byteZero;
            }
            *(u16 *)((u8 *)collider + 0x44) = zero;
            actor->field2B8 = zero;
        }
        {
            u32 mask = 0x20000;
            u32 *flags = &actor->flags2D0;
            *flags |= mask;
        }
    }
    if (frame > 6) {
        actor->body.position.y -= (u32)(frame - 6) * 30;
        if (!actor->drawableKind)
            *(struct ContactPosition *)((u8 *)actor->drawable + 0x1C) = actor->body.position;
        else
            *(struct ContactPosition *)((u8 *)actor->drawable + 0x40) = actor->body.position;
        if (frame > 8) {
            u32 zeroWord = 0;
            u32 zeroByte = 0;
            asm("" : "+r"(zeroWord));
            asm("" : "+r"(zeroByte));
            actor->health = zeroWord;
            {
                register u32 mask asm("r0") = 8;
                u16 *flags = &actor->flags2D4;
                *flags = mask | *flags;
            }
            {
                u32 mask = 0x400;
                u32 *flags = &actor->flags2CC;
                *flags |= mask;
            }
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
            SceneActorEffects_AdvanceSteps(actor, 4);
            {
                void *control = SceneActorReaction_BeginHiddenDefeat;
                u32 kind = 23;
                actor->field2B2 = 1;
                actor->field2B0 = zeroByte;
                actor->field2B5 = kind;
                actor->target = control;
                actor->field2C8 = zeroWord;
                {
                    u32 *flags = &actor->flags114;
                    u32 mask = ~1;
                    *flags &= mask;
                }
                actor->word104 = zeroWord;
            }
            if (!actor->drawableKind)
                *(u32 *)actor->drawable |= 1;
            else {
                struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
                tile->flags |= 1;
            }
        }
    }
}
