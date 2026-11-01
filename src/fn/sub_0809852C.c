#include "scene_actor_control_helpers.h"
#include "engine.h"
struct GhostVisibilityState {
    u8 pad[0x558];
    u32 flags;
};
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
static inline void Hide(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    {
        u32 *flags = actor->drawable;
        u32 value = *flags;
        u32 one = 1;
        *flags = value | one;
    }
    {
        u32 offset = 0x320;
        ActorShadow_Disable((void *)((u8 *)actor + offset));
    }
}

static inline void Show(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    {
        u32 *flags = actor->drawable;
        u32 value = *flags;
        u32 mask;
        mask = ~1;
        *flags = value & mask;
    }
    {
        u32 offset = 0x320;
        ActorShadow_Enable((void *)((u8 *)actor + offset));
    }
}
// confidence=low: flickers the drawable and shadow during a pending visibility transition.
void Ghost_TickVisibilityTransition(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct GhostVisibilityState *state;
    u32 unavailable = IsUnavailable(actor);
    if (unavailable)
        return;
    {
        u32 offset = 0x3D0;
        u32 mask;
        u32 active;
        state = *(void **)((u8 *)actor + offset);
        mask = 0x60;
        offset = 0x558;
        active = SceneActorControl_TestFlags(*(u32 *)((u8 *)state + offset), mask);
        if (!active)
            return;
    }
    {
        /* The engine flags are tested here, but the result goes unused. */
        u8 ignored = SceneActorControl_TestFlags(gEngineFlags, 0x4800);
        u32 counter;
        u32 value;
        {
            u32 offset = 0x5B2;
            u16 *field;
            field = (void *)((u8 *)state + offset);
            counter = (u32)*field << 16;
        }
        value = counter >> 16;
        if (value <= 19) {
            if ((u8)(counter >> 18) & 1)
                Show(actor);
            else
                Hide(actor);
        } else if (value <= 29) {
            if ((u8)(counter >> 17) & 1)
                Show(actor);
            else
                Hide(actor);
        } else {
            u32 mask = 0x20;
            u32 *inputFlags;
            u32 *flags;
            u32 active;
            inputFlags = &state->flags;
            active = *inputFlags & mask;
            flags = inputFlags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (active) {
                {
                    u32 *field = actor->drawable;
                    u32 value = *field;
                    u32 mask = ~1;
                    *field = value & mask;
                }
                {
                    u32 mask = ~0x20;
                    *flags &= mask;
                }
                ActorShadow_Enable((void *)((u8 *)actor + 0x320));
            } else {
                {
                    u32 *field = actor->drawable;
                    u32 value = *field;
                    u32 one = 1;
                    *field = value | one;
                }
                {
                    u32 mask = ~0x40;
                    *flags &= mask;
                }
                ActorShadow_Disable((void *)((u8 *)actor + 0x320));
            }
        }
    }
    {
        u16 *counter = (void *)((u8 *)state + 0x5B2);
        (*counter)++;
    }
}
