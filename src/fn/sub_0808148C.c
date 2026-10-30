// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "engine.h"
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
static inline u8 IsControlled(struct SceneActorRegistryResetView *a)
{
    if (a->object == (void *)1)
        return 1;
    return 0;
}
static inline void ClearRoute(struct SceneActorRegistryResetView *a, u8 *group)
{
    register struct SceneActorRegistryControl *route asm("r2") = (void *)(group + 0x4C);
    register u32 *flags asm("r3") = &a->flags2CC;
    register u32 mask asm("r1") = ~0x141C0;
    *flags &= mask;
    route->counter = 0;
    *(u16 *)((u8 *)route + 0x14) = 0;
    route->timer = 0;
}
void SceneActorReaction_PlayStaggerBurst(struct SceneActorRegistryResetView *input, s32 inputFrame)
{
    struct SceneActorRegistryResetView *actor = input;
    s32 frame = inputFrame;
    u8 *group = (u8 *)actor + 0x3D4;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 category = actor->category;
        u32 randomMask;
        /* Named components are initialized; aggregate padding is never read as a scalar. */
        union {
            struct ContactPackedVector packed;
            struct ContactPosition position;
        } range;
        if (IsControlled(actor))
            ClearRoute(actor, group);
        if (category == 22)
            SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        else
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            if (!actor->drawableKind) {
                u32 *flags = actor->drawable;
                u32 mask = ~1;
                *flags &= mask;
            } else {
                struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
                u32 mask = ~1;
                tile->flags &= mask;
            }
        }
        {
            register u32 state asm("r1") = 3;
            u8 *field = &actor->state2A8;
            u32 zero = 0;
            randomMask = 3;
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
                actor->field2B3 = byteZero;
            }
        }

        {
            u16 *fields = &actor->flags2D4;
            u32 mask;
            register u32 value asm("r1");
            mask = ~0x20;
            value = *fields;
            *fields = mask & value;
        }
        SceneActorPlacement_PlayCategoryResponseSound((void *)actor);
        Sound_PlaySong(298);
        range.packed.x = 60;
        range.packed.y = 60;
        range.packed.z = 40;
        {
            u32 index = (gRandomIndex + 1) & 1023;
            u32 mode;
            gRandomIndex = index;
            index *= 2;
            mode = *(const u16 *)((u32)gRandomTable + index) & randomMask;
            mode += 1;
            SparkParticle_Spawn(&actor->body.position, mode, &range.position, actor->body.direction + 128, 10,
                                6, 2, 2);
        }
    }
    if (!actor->drawableKind) {
        u8 *sequence = (u8 *)actor->drawable + 0x48;
        *(u16 *)(sequence + 0xE) = 0;
    } else
        *(u16 *)((u8 *)actor->drawable + 0x2C) = 0;
    if (frame > 15) {
        u32 mask = 2;
        u32 flagOffset = 0x2D4;
        u16 *flags;
        u16 masked;
        flags = (u16 *)((u8 *)actor + flagOffset);
        {
            u32 raw = *flags & mask;
            masked = (u16)raw;
        }
        if (masked) {
            register u32 state asm("r1") = 3;
            u32 stateOffset = 0x2A8;
            u8 *field;
            u32 phaseOffset;
            u32 one;
            void *control;
            field = (u8 *)actor + stateOffset;
            /* The address temporary is dead; retain its register for the zero stores. */
            stateOffset = 0;
            *field = state;
            phaseOffset = 0x2A9;
            field = (u8 *)actor + phaseOffset;
            *field = stateOffset;
            {
                register u32 offset asm("r1") = 0x2AA;
                asm("" : : "r"(offset));
                field = (u8 *)actor + offset;
            }
            *field = stateOffset;
            flagOffset -= 0x28;
            *(u32 *)((u8 *)actor + flagOffset) = stateOffset;
            phaseOffset += 8;
            field = (u8 *)actor + phaseOffset;
            one = 1;
            *field = one;
            flagOffset -= 0xA4;
            {
                void **source = (void **)(group + flagOffset);
                control = *source;
            }
            {
                u32 kind = 7;
                u32 controlOffset;
                phaseOffset += 1;
                *((u8 *)actor + phaseOffset) = one;
                {
                    u8 *field;
                    controlOffset = 0x2B0;
                    field = (u8 *)actor + controlOffset;
                    *field = stateOffset;
                }
                {
                    register u8 *field asm("r0");
                    phaseOffset += 3;
                    field = (u8 *)actor + phaseOffset;
                    *field = kind;
                }
                {
                    register void **field asm("r0");
                    controlOffset -= 0x94;
                    field = (void **)((u8 *)actor + controlOffset);
                    *field = control;
                }
                {
                    register u32 offset asm("r2") = 0x2C8;
                    register u16 *field asm("r0");
                    asm("" : : "r"(offset));
                    field = (u16 *)((u8 *)actor + offset);
                    *field = stateOffset;
                }
                {
                    u32 *fields;
                    u32 clear;
                    phaseOffset = 0x114;
                    asm("" : : "r"(phaseOffset));
                    fields = (u32 *)((u8 *)actor + phaseOffset);
                    clear = ~1;
                    *fields &= clear;
                }
                {
                    u32 *field = &actor->word104;
                    *field = stateOffset;
                }
            }
        } else {
            void *control = *(void **)((u8 *)actor + 0x230);
            u32 kind = 18;
            actor->field2B2 = 1;
            actor->field2B0 = masked;
            actor->field2B5 = kind;
            actor->target = control;
            actor->field2C8 = masked;
            {
                u32 *fields = &actor->flags114;
                u32 clear = ~1;
                *fields &= clear;
            }
            actor->word104 = masked;
            {
                u32 state = 3;
                u8 *field = &actor->state2A8;
                *field = state;
                actor->phase2A9 = 0;
                actor->behavior2AA = 0;
                actor->counter2AC = masked;
                {
                    register u32 offset asm("r6") = 0x2B1;
                    register u8 *ready asm("r0");
                    asm("" : : "r"(offset));
                    ready = (u8 *)actor + offset;
                    *ready = 1;
                }
            }
        }
    }
}
