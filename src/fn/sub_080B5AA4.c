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
static inline u8 IsHidden(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 1;
    if (!actor->drawableKind) {
        if (*(u32 *)actor->drawable & mask)
            goto hidden;
        goto visible;
    }
    if (!(*(u32 *)((u8 *)actor->drawable + 0x28) & mask))
        goto visible;
hidden:
    return 1;
visible:
    return 0;
}
/* actor must be nonnull: its control context is loaded before the availability gate. */

void sub_080B5AA4(struct SceneActorRegistryResetView *input, const u16 *inputRequest)
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
            u32 mask = 0x4000;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = (u32 *)((u8 *)state + 0x6DC);
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = IsHidden(actor);
                if (hidden) {
                    {
                        u32 offset = 0x748;
                        u16 *timer;
                        timer = (void *)((u8 *)state + offset);
                        *timer = 0;
                    }
                    {
                        u32 mask = ~0x18000;
                        {
                            u32 old = *savedFlags & mask;
                            u32 bits = 0x4000;
                            *savedFlags = old | bits;
                        }
                    }
                    Sound_PlaySong(0x21E);
                }
            }
        } else {
            u32 mask = 0x8000;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = (u32 *)((u8 *)state + 0x6DC);
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = IsHidden(actor);
                if (!hidden) {
                    u32 hiddenZero = hidden;
                    {
                        u32 zero = hiddenZero;
                        u32 offset = 0x748;
                        u16 *timer;
                        timer = (void *)((u8 *)state + offset);
                        *timer = zero;
                    }
                    {
                        u32 mask = ~0x14000;
                        {
                            u32 old = *savedFlags & mask;
                            u32 bits = 0x8000;
                            *savedFlags = old | bits;
                        }
                    }
                    Sound_PlaySong(0x21E);
                }
            }
        }
    }
}
