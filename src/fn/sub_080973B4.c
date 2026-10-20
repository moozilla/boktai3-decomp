// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
static inline u32 ReadFlags16(struct SceneActorRegistryResetView *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u32 offset = 0x2D4;
    u16 *flags;
    flags = (void *)((u8 *)actor + offset);
    return *flags & mask;
}
static inline u32 ReadFlags32(struct SceneActorRegistryResetView *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u32 offset = 0x2D0;
    u32 *flags;
    flags = (void *)((u8 *)actor + offset);
    return *flags & mask;
}
static inline u8 IsUnavailable(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    if (actor) {
        if (ReadFlags16(actor, 0x40) || ReadFlags32(actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u32 IsHidden(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u32 mask = 1;
    register u32 result asm("r0");
    if (actor->drawableKind == 0) {
        u32 flags = *(u32 *)actor->drawable & mask;
        if (flags) {
            result = 1;
            goto done;
        }
        asm("" ::: "r1");
    } else {
        u32 flags = *(u32 *)((u8 *)actor->drawable + 0x28) & mask;
        if (flags) {
            result = 1;
            goto done;
        }
    }
    result = 0;
done:
    return result;
}
/* actor must be nonnull: its control context is loaded before the availability gate. */
#define Ghost_RequestVisibility sub_080973B4

void Ghost_RequestVisibility(struct SceneActorRegistryResetView *input, const u16 *inputRequest)
{
    struct SceneActorRegistryResetView *actor = input;
    const u16 *request = inputRequest;
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    u32 unavailable = IsUnavailable(actor);
    if (unavailable)
        return;
    {
        u32 mask = 0x4000;
        u16 *flags;
        flags = &actor->flags2D4;
        if (!(*flags & mask))
            return;
    }
    {
        u32 requested = request[1];
        if (requested == 0) {
            u32 mask = 0x20;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = (u32 *)((u8 *)state + 0x558);
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = IsHidden(actor);
                asm("" : "+r"(hidden));
                if (hidden) {
                    {
                        u32 offset = 0x5B2;
                        u16 *timer;
                        timer = (void *)((u8 *)state + offset);
                        *timer = 0;
                    }
                    {
                        u32 mask = ~0x240;
                        {
                            u32 old = *savedFlags & mask;
                            u32 bits = 0x20;
                            *savedFlags = old | bits;
                        }
                    }
                    Sound_PlaySong(0x21E);
                    {
                        u32 mask = 0x800;
                        u16 *flags;
                        flags = &actor->flags2C0;
                        mask |= *flags;
                        *flags = mask;
                    }
                }
            }
        } else {
            u32 mask = 0x40;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = (u32 *)((u8 *)state + 0x558);
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = IsHidden(actor);
                if (!hidden) {
                    u32 hiddenZero = hidden;
                    {
                        u32 zero = hiddenZero;
                        u32 offset = 0x5B2;
                        u16 *timer;
                        timer = (void *)((u8 *)state + offset);
                        *timer = zero;
                    }
                    {
                        u32 mask = ~0x220;
                        {
                            u32 old = *savedFlags & mask;
                            u32 bits = 0x40;
                            *savedFlags = old | bits;
                        }
                    }
                    Sound_PlaySong(0x21E);
                    {
                        u32 mask = 0x800;
                        u16 *flags;
                        flags = &actor->flags2C0;
                        mask |= *flags;
                        *flags = mask;
                    }
                }
            }
        }
    }
}
