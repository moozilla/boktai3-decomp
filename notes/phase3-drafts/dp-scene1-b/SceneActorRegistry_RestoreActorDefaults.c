#include "scene_actor_terrain_reaction.h"
#include "scene_actor_reset.h"
#define ACTOR_OFFSET(member) ((u32) & ((struct SceneActorRegistryResetView *)0)->member)
/* Offset temporaries preserve pointer lifetimes required by the original allocator. */
void SceneActorControl_OnIdle(u8 *);
void SceneActorPalette_ClearCompletedTransitions(void);
/* The latter two arguments are observed but unused by this initializer. */
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void *ActorAttachment_ReleaseResource(void *, u32, u32);
void *ActorAttachment_ReleasePartsResource(void *, u32, u32);
extern const u8 gUnk_0824E0D8[];
static inline u8 SceneActorRegistry_HasNoObject(struct SceneActorRegistryResetView *actor)
{
    if (!actor->object)
        return 1;
    return 0;
}
void SceneActorRegistry_RestoreActorDefaults(struct SceneActorRegistryActor *argument)
{
    struct SceneActorRegistryResetView *actor = (void *)argument;
    u32 *flags;
    u32 noObject;
    SceneActorRegistry_ResetRouteAndColliders((void *)actor);
    SceneActorTerrain_EnterState(actor, 3, 0, 0);
    SceneActorTerrain_InstallControl(actor, SceneActorControl_OnIdle, 16);
    {
        u32 result = SceneActorRegistry_HasNoObject(actor);
        noObject = result;
    }
    if (noObject) {
        u32 *branchFlags = &actor->flags2CC;
        s32 health;
        actor->flags2CC = actor->saved.behaviorFlags;
        actor->flags2D0 = actor->saved.statusFlags;
        actor->body.direction = actor->saved.direction;
        actor->state2A8 = actor->saved.state;
        actor->phase2A9 = actor->saved.phase;
        actor->behavior2AA = actor->saved.behavior;
        actor->target = actor->saved.control;
        actor->field2B5 = actor->saved.kind;
        actor->flags2C0 = actor->saved.motionFlags;
        actor->field2EA = actor->saved.heightOffset;
        health = actor->defaultHealth;
        flags = branchFlags;
        if (health <= 0) health = 0;
        actor->health = health;
        actor->flags2D6 = 0;
        actor->flags2D4 = 0;
        actor->field2B0 = 0;
        actor->field2B1 = 1;
        actor->field2B2 = 1;
        actor->field2B3 = 0;
        actor->field304 = 0;
        actor->field2B6 = 0;
        actor->field2C2 = 64;
        actor->field2D8 = 0;
        actor->field2DC = 0;
        actor->field2E0 = 0;
        actor->field2E6 = 0;
        actor->field3CC = 0;
        actor->tracking.target = 0;
        SceneActorPalette_ClearCompletedTransitions();
    } else {
        u32 wordZero;
        u32 byteZero;
        u32 parameterOffset;
        u32 fieldOffset;
        u32 *branchFlags;
        {
            register u32 offset = ACTOR_OFFSET(flags2CC);
            branchFlags = (u32 *)((u8 *)actor + offset);
        }
        {
            u32 sourceOffset = ACTOR_OFFSET(saved.behaviorFlags);
            u32 value;
            {
                u32 *source = (u32 *)((u8 *)actor + sourceOffset);
                value = *source;
            }
            *branchFlags = value;
        }
        {
            u32 offset = ACTOR_OFFSET(flags2D0);
            u32 *address;
            register u32 sourceOffset;
            u32 value;
            address = (u32 *)((u8 *)actor + offset);
            sourceOffset = ACTOR_OFFSET(saved.statusFlags);
            {
                register u32 *source = (u32 *)((u8 *)actor + sourceOffset);
                value = *source;
            }
            *address = value;
        }
        parameterOffset = ACTOR_OFFSET(saved.direction);
        {
            u32 direction = *((u8 *)actor + parameterOffset);

            actor->body.direction = direction;
        }
        {
            u32 sourceOffset = ACTOR_OFFSET(saved.state);
            u8 *address;
            u32 value;
            address = (u8 *)actor + sourceOffset;
            value = *address;
            fieldOffset = ACTOR_OFFSET(state2A8);
            actor->state2A8 = value;
            parameterOffset += 2;
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset += 1;
            actor->phase2A9 = value;
            parameterOffset += 1;
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset += 1;
            address = (u8 *)actor + fieldOffset;
            actor->behavior2AA = value;
        }
        parameterOffset += 0xF9;
        {
            const void **address = (const void **)((u8 *)actor + parameterOffset);
            const void *value;
            fieldOffset = ACTOR_OFFSET(saved.control);
            {
                const void **source = (const void **)((u8 *)actor + fieldOffset);
                value = *source;
            }
            *address = value;
        }
        parameterOffset -= 0xF4;
        {
            u8 *address;
            register u32 value;
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset = ACTOR_OFFSET(field2B5);
            actor->field2B5 = value;
        }
        parameterOffset += 6;
        {
            u16 *address;
            u32 value;
            address = (u16 *)((u8 *)actor + parameterOffset);
            value = *address;
            fieldOffset += 0xB;
            address = (u16 *)((u8 *)actor + fieldOffset);
            byteZero = 0;
            actor->flags2C0 = value;
            parameterOffset -= 2;
            address = (u16 *)((u8 *)actor + parameterOffset);
            value = *address;
            fieldOffset += 0x2A;
            address = (u16 *)((u8 *)actor + fieldOffset);
            actor->field2EA = value;
        }
        {
            s32 health = actor->defaultHealth;
            flags = branchFlags;
            if (health <= 0)
                health = 0;
            actor->health = health;
        }
        wordZero = noObject;
        {
            u32 offset1;
            u32 offset2;
            u8 *address;
            u32 one;
            actor->flags2D6 = wordZero;
            offset2 = ACTOR_OFFSET(flags2D4);
            actor->flags2D4 = wordZero;
            fieldOffset = ACTOR_OFFSET(field2B0);
            actor->field2B0 = byteZero;
            parameterOffset = 0x2B1;
            address = (u8 *)actor + parameterOffset;
            one = 1;
            actor->field2B1 = one;
            offset2 -= 0x22;
            actor->field2B2 = one;
            fieldOffset += 3;
            actor->field2B3 = byteZero;
            parameterOffset = ACTOR_OFFSET(group.common.gate.flags);
            address = (u8 *)actor + parameterOffset;
            *(u16 *)address = wordZero;
            actor->group.common.gate.counter = wordZero;
            {
                u16 *address = (u16 *)((u8 *)actor + 0xAA);
                *address = wordZero;
            }
            offset2 += 0x52;
            actor->field304 = wordZero;
            fieldOffset += 3;
            actor->field2B6 = byteZero;
            parameterOffset = ACTOR_OFFSET(field2C2);
            actor->field2C2 = 64;
            offset1 = ACTOR_OFFSET(field2D8);
            actor->field2D8 = wordZero;
            offset2 -= 0x28;
            actor->field2DC = wordZero;
            fieldOffset += 0x2A;
            actor->field2E0 = wordZero;
            parameterOffset += 0x24;
            actor->field2E6 = wordZero;
            offset1 += 0xF4;
            actor->field3CC = byteZero;
            actor->tracking.target = wordZero;
            offset2 -= 0x24;
            address = (u8 *)actor + offset2;
            actor->field2B8 = wordZero;
            fieldOffset = ACTOR_OFFSET(group.hitReactionLatch);
            address = (u8 *)actor + fieldOffset;
            *address = byteZero;
            parameterOffset = ACTOR_OFFSET(group.field5C6);
            actor->group.field5C6 = wordZero;
            offset1 += 0x5E;
            address = (u8 *)actor + offset1;
            actor->group.common.control.mode = byteZero;
        }
        {
            register u32 offset2 = ACTOR_OFFSET(group.motion5B8);
            Memory_Clear((u8 *)actor + offset2, 8);
        }
        {
            register u32 offset3 = ACTOR_OFFSET(group.field5C4);
            u16 *address;
            register u32 offset1;
            u32 offset2;
            u32 *sequence;
            address = (u16 *)((u8 *)actor + offset3);
            *address = wordZero;
            parameterOffset -= 6;
            actor->group.field5C0 = wordZero;
            offset1 = ACTOR_OFFSET(group.field5C2);
            address = (u16 *)((u8 *)actor + offset1);
            *address = wordZero;
            offset2 = ACTOR_OFFSET(group.saved.control);
            sequence = (u32 *)((u8 *)actor + offset2);
            {
                const void *resource = gUnk_0824E0D8;
                void *destination;
                destination = sequence;
                offset2 -= 0x85;
                SceneActorReaction_ResetResponseState(destination, resource, offset2);
            }
            offset3 = ACTOR_OFFSET(group.turn.direction);
            {
                register u8 *address = (u8 *)actor + offset3;
                *address = byteZero;
            }
            offset1 = ACTOR_OFFSET(group.turn.savedDirection);
            {
                u8 *address = (u8 *)actor + offset1;
                *address = byteZero;
            }
            offset2 = ACTOR_OFFSET(group.turn.turnsRemaining);
            {
                u8 *byteAddress = (u8 *)actor + offset2;
                *byteAddress = 255;
            }
            *sequence = wordZero;
            offset3 = ACTOR_OFFSET(group.saved.state);
            {
                register u8 *address = (u8 *)actor + offset3;
                *address = byteZero;
            }
        }
        {
            u32 offset4 = ACTOR_OFFSET(group.saved.phase);
            u32 offset6;
            u32 offset1;
            {
                u8 *address = (u8 *)actor + offset4;
                *address = byteZero;
            }
            offset6 = ACTOR_OFFSET(group.saved.behavior);
            {
                u8 *address = (u8 *)actor + offset6;
                *address = byteZero;
            }
            offset1 = ACTOR_OFFSET(group.saved.kind);
            {
                u8 *address = (u8 *)actor + offset1;
                *address = byteZero;
            }
            SceneActorPalette_ClearCompletedTransitions();
        }
        {
            register u32 offset2 = ACTOR_OFFSET(group.attachment);
            void **address;
            void *attachment;
            address = (void **)((u8 *)actor + offset2);
            attachment = *address;
            attachment = ActorAttachment_ReleaseResource(attachment, 1, actor->body.id);
            *address = attachment;
            attachment = ActorAttachment_ReleasePartsResource(attachment, 3, actor->body.id);
            *address = attachment;
        }
    }
    {
        u32 two = 2;
        register u32 *address;
        u32 value;
        address = flags;
        value = *address;
        value |= two;
        *address = value;
    }
    {
        u32 offset = ACTOR_OFFSET(colliderA);
        struct ContactCollider *collider;
        u32 previous;
        u32 value;
        collider = (struct ContactCollider *)((u8 *)actor + offset);
        previous = collider->flags;
        value = 4;
        value |= previous;
        collider->flags = value;
    }
    {
        u32 offset = ACTOR_OFFSET(callback248);
        void *callback;
        void **address;
        address = (void **)((u8 *)actor + offset);
        callback = *address;
        if (callback)
            Callback_InvokeActor(actor, callback);
    }
    SceneActorPalette_ClearCompletedTransitions();
}
