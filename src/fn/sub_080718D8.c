#include "scene_actor_terrain_reaction.h"
// MATCHING_CONCESSION: register-pinning
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
    if (!actor->base.object)
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
        u32 wordZero;
        register u32 byteZero asm("r4");
        register u32 parameterOffset asm("r6");
        register u32 fieldOffset asm("r4");
        u32 *branchFlags = &actor->base.flags2CC;
        {
            u32 sourceOffset = ACTOR_OFFSET(base.saved.behaviorFlags);
            u32 value;
            {
                u32 *source = (u32 *)((u8 *)actor + sourceOffset);
                value = *source;
            }
            actor->base.flags2CC = value;
        }
        {
            register u32 destinationOffset asm("r3") = ACTOR_OFFSET(base.flags2D0);
            register u32 *address asm("r1");
            register u32 sourceOffset asm("r4");
            u32 value;
            asm("" : "+r"(destinationOffset));
            address = (u32 *)((u8 *)actor + destinationOffset);
            sourceOffset = ACTOR_OFFSET(base.saved.statusFlags);
            asm("" : "+r"(sourceOffset));
            {
                register u32 *source asm("r0") = (u32 *)((u8 *)actor + sourceOffset);
                value = *source;
            }
            *address = value;
        }
        parameterOffset = ACTOR_OFFSET(base.saved.direction);
        {
            u32 direction = *((u8 *)actor + parameterOffset);
            u8 *destination = &actor->base.body.direction;
            wordZero = 0;
            *destination = direction;
        }
        {
            register u32 sourceOffset asm("r1") = ACTOR_OFFSET(base.saved.state);
            u8 *address;
            u32 value;
            asm("" : "+r"(sourceOffset));
            address = (u8 *)actor + sourceOffset;
            value = *address;
            fieldOffset = ACTOR_OFFSET(base.state2A8);
            actor->base.state2A8 = value;
            parameterOffset += 2;
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset += 1;
            actor->base.phase2A9 = value;
            parameterOffset += 1;
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset += 1;
            address = (u8 *)actor + fieldOffset;
            actor->base.behavior2AA = value;
        }
        parameterOffset += 0xF9;
        {
            const void **address = (const void **)((u8 *)actor + parameterOffset);
            const void *value;
            fieldOffset = ACTOR_OFFSET(base.saved.control);
            {
                const void **source = (const void **)((u8 *)actor + fieldOffset);
                value = *source;
            }
            *address = value;
        }
        parameterOffset -= 0xF4;
        {
            u8 *address;
            u32 value;
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset = ACTOR_OFFSET(base.field2B5);
            address = (u8 *)actor + fieldOffset;
            actor->base.field2B5 = value;
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
            *address = value;
            parameterOffset -= 2;
            address = (u16 *)((u8 *)actor + parameterOffset);
            value = *address;
            parameterOffset = ACTOR_OFFSET(base.field2EA);
            address = (u16 *)((u8 *)actor + parameterOffset);
            actor->base.field2EA = value;
        }
        {
            s32 health = actor->base.defaultHealth;
            flags = branchFlags;
            if (health <= 0)
                health = 0;
            actor->base.health = health;
        }
        {
            u32 offset1 = ACTOR_OFFSET(base.flags2D6);
            u32 offset2;
            u8 *address;
            register u32 one asm("r1");
            actor->base.flags2D6 = wordZero;
            offset2 = ACTOR_OFFSET(base.flags2D4);
            actor->base.flags2D4 = wordZero;
            parameterOffset = ACTOR_OFFSET(base.field2B0);
            actor->base.field2B0 = byteZero;
            offset1 -= 0x25;
            address = (u8 *)actor + offset1;
            one = 1;
            actor->base.field2B1 = one;
            offset2 -= 0x22;
            actor->base.field2B2 = one;
            parameterOffset += 3;
            address = (u8 *)actor + parameterOffset;
            *address = byteZero;
            actor->base.field304 = wordZero;
            offset2 += 4;
            address = (u8 *)actor + offset2;
            *address = byteZero;
            parameterOffset += 0xF;
            {
                u16 *timerAddress = (u16 *)((u8 *)actor + parameterOffset);
                *timerAddress = 64;
            }
            offset1 = ACTOR_OFFSET(base.field2D8);
            actor->base.field2D8 = wordZero;
            offset2 += 0x26;
            actor->base.field2DC = wordZero;
            parameterOffset += 0x1E;
            address = (u8 *)actor + parameterOffset;
            *(u32 *)address = wordZero;
            offset1 += 0xE;
            actor->base.field2E6 = wordZero;
            offset2 += 0xF0;
            address = (u8 *)actor + offset2;
            *address = byteZero;
            actor->base.tracking.target = wordZero;
        }
        SceneActorPalette_ClearCompletedTransitions();
    } else {
        u32 wordZero;
        u32 byteZero;
        u32 parameterOffset;
        u32 fieldOffset;
        u32 *branchFlags;
        {
            register u32 offset asm("r3") = ACTOR_OFFSET(base.flags2CC);
            asm("" : "+r"(offset));
            branchFlags = (u32 *)((u8 *)actor + offset);
        }
        {
            u32 sourceOffset = ACTOR_OFFSET(base.saved.behaviorFlags);
            u32 value;
            {
                u32 *source = (u32 *)((u8 *)actor + sourceOffset);
                value = *source;
            }
            *branchFlags = value;
        }
        {
            u32 offset = ACTOR_OFFSET(base.flags2D0);
            u32 *address;
            register u32 sourceOffset asm("r3");
            u32 value;
            address = (u32 *)((u8 *)actor + offset);
            sourceOffset = ACTOR_OFFSET(base.saved.statusFlags);
            asm("" : "+r"(sourceOffset));
            {
                register u32 *source asm("r0") = (u32 *)((u8 *)actor + sourceOffset);
                value = *source;
            }
            *address = value;
        }
        parameterOffset = ACTOR_OFFSET(base.saved.direction);
        {
            u32 direction = *((u8 *)actor + parameterOffset);

            actor->base.body.direction = direction;
        }
        {
            u32 sourceOffset = ACTOR_OFFSET(base.saved.state);
            u8 *address;
            u32 value;
            address = (u8 *)actor + sourceOffset;
            value = *address;
            fieldOffset = ACTOR_OFFSET(base.state2A8);
            actor->base.state2A8 = value;
            parameterOffset += 2;
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset += 1;
            actor->base.phase2A9 = value;
            parameterOffset += 1;
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset += 1;
            address = (u8 *)actor + fieldOffset;
            actor->base.behavior2AA = value;
        }
        parameterOffset += 0xF9;
        {
            const void **address = (const void **)((u8 *)actor + parameterOffset);
            const void *value;
            fieldOffset = ACTOR_OFFSET(base.saved.control);
            asm("" : "+r"(fieldOffset));
            {
                const void **source = (const void **)((u8 *)actor + fieldOffset);
                value = *source;
            }
            *address = value;
        }
        parameterOffset -= 0xF4;
        {
            u8 *address;
            register u32 value asm("r1");
            address = (u8 *)actor + parameterOffset;
            value = *address;
            fieldOffset = ACTOR_OFFSET(base.field2B5);
            address = (u8 *)actor + fieldOffset;
            actor->base.field2B5 = value;
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
            actor->base.flags2C0 = value;
            parameterOffset -= 2;
            address = (u16 *)((u8 *)actor + parameterOffset);
            value = *address;
            fieldOffset += 0x2A;
            address = (u16 *)((u8 *)actor + fieldOffset);
            actor->base.field2EA = value;
        }
        {
            s32 health = actor->base.defaultHealth;
            asm("" : : "r"(fieldOffset));
            flags = branchFlags;
            if (health <= 0)
                health = 0;
            actor->base.health = health;
        }
        wordZero = noObject;
        {
            u32 offset1;
            u32 offset2;
            u8 *address;
            u32 one;
            actor->base.flags2D6 = wordZero;
            offset2 = ACTOR_OFFSET(base.flags2D4);
            actor->base.flags2D4 = wordZero;
            fieldOffset = ACTOR_OFFSET(base.field2B0);
            actor->base.field2B0 = byteZero;
            parameterOffset = 0x2B1;
            address = (u8 *)actor + parameterOffset;
            one = 1;
            actor->base.field2B1 = one;
            offset2 -= 0x22;
            actor->base.field2B2 = one;
            fieldOffset += 3;
            actor->base.field2B3 = byteZero;
            parameterOffset = ACTOR_OFFSET(group.common.gate.flags);
            asm("" : "+r"(parameterOffset));
            address = (u8 *)actor + parameterOffset;
            *(u16 *)address = wordZero;
            actor->group.common.gate.counter = wordZero;
            {
                u16 *address = (u16 *)((u8 *)actor + 0xAA);
                *address = wordZero;
            }
            offset2 += 0x52;
            actor->base.field304 = wordZero;
            fieldOffset += 3;
            actor->base.field2B6 = byteZero;
            parameterOffset = ACTOR_OFFSET(base.field2C2);
            actor->base.field2C2 = 64;
            offset1 = ACTOR_OFFSET(base.field2D8);
            actor->base.field2D8 = wordZero;
            offset2 -= 0x28;
            actor->base.field2DC = wordZero;
            fieldOffset += 0x2A;
            actor->base.field2E0 = wordZero;
            parameterOffset += 0x24;
            actor->base.field2E6 = wordZero;
            offset1 += 0xF4;
            actor->base.field3CC = byteZero;
            actor->base.tracking.target = wordZero;
            offset2 -= 0x24;
            address = (u8 *)actor + offset2;
            actor->base.field2B8 = wordZero;
            fieldOffset = ACTOR_OFFSET(group.hitReactionLatch);
            asm("" : "+r"(fieldOffset));
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
            register u32 offset3 asm("r3") = ACTOR_OFFSET(group.field5C4);
            u16 *address;
            register u32 offset1 asm("r1");
            u32 offset2;
            u32 *sequence;
            asm("" : "+r"(offset3));
            address = (u16 *)((u8 *)actor + offset3);
            *address = wordZero;
            parameterOffset -= 6;
            actor->group.field5C0 = wordZero;
            offset1 = ACTOR_OFFSET(group.field5C2);
            asm("" : "+r"(offset1));
            address = (u16 *)((u8 *)actor + offset1);
            *address = wordZero;
            offset2 = ACTOR_OFFSET(group.saved.control);
            sequence = (u32 *)((u8 *)actor + offset2);
            {
                const void *resource = gUnk_0824E0D8;
                void *destination;
                destination = sequence;
                asm("" : "+r"(destination));
                offset2 -= 0x85;
                SceneActorReaction_ResetResponseState(destination, resource, offset2);
            }
            offset3 = ACTOR_OFFSET(group.turn.direction);
            asm("" : "+r"(offset3));
            {
                register u8 *address asm("r0") = (u8 *)actor + offset3;
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
            asm("" : "+r"(offset3));
            {
                register u8 *address asm("r0") = (u8 *)actor + offset3;
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
            attachment = ActorAttachment_ReleaseResource(attachment, 1, actor->base.body.id);
            *address = attachment;
            attachment = ActorAttachment_ReleasePartsResource(attachment, 3, actor->base.body.id);
            *address = attachment;
        }
    }
    {
        u32 two = 2;
        register u32 *address asm("r3");
        u32 value;
        address = flags;
        value = *address;
        value |= two;
        *address = value;
    }
    {
        u32 offset = ACTOR_OFFSET(base.colliderA);
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
        u32 offset = ACTOR_OFFSET(base.callback248);
        void *callback;
        void **address;
        address = (void **)((u8 *)actor + offset);
        callback = *address;
        if (callback)
            Callback_InvokeActor(actor, callback);
    }
    SceneActorPalette_ClearCompletedTransitions();
}
