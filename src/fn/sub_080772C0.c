// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
static inline u32 Unavailable(struct SceneActorRegistryResetView *actor, u32 flags)
{
    u32 result;
    register u32 test asm("r2");
    u32 mask;
    if (!actor)
        goto available;
    {
        test = flags & 0x40;
        if (test)
            goto unavailable;
    }
    {
        mask = 0x20000;
        test = 0x2D0;
        if (*(u32 *)((u8 *)actor + test) & mask)
            goto unavailable;
    }
    {
        s16 *health = &actor->health;
        s32 value;
        value = *health;
        asm("" : "+r"(value), "+r"(test), "+r"(mask));
        if (value > 0)
            goto available;
    }
unavailable:
    result = 1;
    goto done;
available:
    result = 0;
done:
    asm("" : "+r"(result));
    return result;
}
void sub_080772C0(struct SceneActorRegistryResetView *actor,
                                        const u16 *inputArgument)
{
    const u16 *argument = inputArgument;
    u32 prior, currentFlags;
    register u32 mode asm("r5");
    register u32 zero asm("r6");
    {
        u32 mask = 0x800;
        u16 *flags;
        flags = &actor->flags2D4;
        {
            register u32 loaded asm("r3") = *flags;
            prior = (u16)(mask & loaded);
            if (prior)
                return;
            zero = 0;
            currentFlags = mask | loaded;
            *flags = currentFlags;
        }
    }
    mode = *argument;
    switch (mode) {
        default: {
            u32 mask = 1;
            if (actor->flags2CC & mask) {
                register u32 offset asm("r3");
                SceneActorRegistry_RestoreActorDefaults((void *)actor);
                offset = 0x178;
                {
                    u16 *field = (void *)((u8 *)actor + offset);
                    *field = prior;
                }
            }
            return;
        }
        case 0: {
            u32 *flags;
            u32 value;
            prior = 1;
            {
                u32 offset = 0x2CC;
                flags = (void *)((u8 *)actor + offset);
            }
            value = *flags;
            if (!(value & prior))
                return;
            *flags = value & ~3;
            SceneActorRegistry_ResetRouteAndColliders((void *)actor);
            {
                register u32 state asm("r1") = 3;
                register u32 stateOffset asm("r2") = 0x2A8;
                u32 phaseOffset;
                u32 behaviorOffset;
                void *control;
                *((u8 *)actor + stateOffset) = state;
                phaseOffset = 0x2A9;
                *((u8 *)actor + phaseOffset) = zero;
                behaviorOffset = 0x2AA;
                *((u8 *)actor + behaviorOffset) = zero;
                stateOffset += 4;
                asm("" : "+r"(stateOffset));
                {
                    register u32 *field asm("r0") = (void *)((u8 *)actor + stateOffset);
                    *field = mode;
                }
                phaseOffset += 8;
                *((u8 *)actor + phaseOffset) = prior;
                behaviorOffset -= 0x8A;
                {
                    void **field = (void *)((u8 *)actor + behaviorOffset);
                    control = *field;
                }
                state = 12;
                phaseOffset++;
                *((u8 *)actor + phaseOffset) = prior;
                phaseOffset -= 2;
                {
                    register u8 *field asm("r0") = (u8 *)actor + phaseOffset;
                    *field = zero;
                }
                {
                    register u32 kindOffset asm("r6") = 0x2B5;
                    asm("" : "+r"(kindOffset));
                    {
                        register u8 *field asm("r0") = (u8 *)actor + kindOffset;
                        *field = state;
                    }
                }
                behaviorOffset = 0x21C;
                asm("" : "+r"(behaviorOffset));
                {
                    void **field = (void *)((u8 *)actor + behaviorOffset);
                    *field = control;
                }
            }
            break;
        }
        case 1: {
            register u32 offset asm("r6");
            u32 *flags;
            register u32 mask asm("r1");
            mode = Unavailable(actor, currentFlags);
            if (mode)
                return;
            offset = 0x2CC;
            flags = (void *)((u8 *)actor + offset);
            mask = ~3;
            *flags &= mask;
            SceneActorRegistry_ResetRoute((void *)actor);
            {
                register u32 state asm("r1") = 3;
                u32 stateOffset = 0x2A8;
                register u32 phaseOffset asm("r3");
                register void *control asm("r3");
                *((u8 *)actor + stateOffset) = state;
                phaseOffset = 0x2A9;
                *((u8 *)actor + phaseOffset) = mode;
                offset -= 0x22;
                *((u8 *)actor + offset) = mode;
                state = 0x2AC;
                *(u32 *)((u8 *)actor + state) = mode;
                stateOffset += 9;
                {
                    u8 *field = (u8 *)actor + stateOffset;
                    *field = 1;
                }
                phaseOffset -= 0x89;
                {
                    register void **field asm("r0") = (void *)((u8 *)actor + phaseOffset);
                    control = *field;
                }
                stateOffset = 12;
                offset += 8;
                *((u8 *)actor + offset) = 1;
                state = 0x2B0;
                *((u8 *)actor + state) = mode;
                offset += 3;
                {
                    register u8 *field asm("r0") = (u8 *)actor + offset;
                    *field = stateOffset;
                }
                state -= 0x94;
                {
                    register void **field asm("r0") = (void *)((u8 *)actor + state);
                    *field = control;
                }
            }
            break;
        }
    }
    actor->field2C8 = mode;
    {
        register u32 offset asm("r3") = 0x114;
        register u32 *field asm("r2");
        u32 mask;
        field = (void *)((u8 *)actor + offset);
        mask = ~1;
        *field &= mask;
    }
    {
        register u32 offset asm("r6") = 0x104;
        asm("" : "+r"(offset));
        {
            register u32 *field asm("r0") = (void *)((u8 *)actor + offset);
            *field = mode;
        }
    }
}
