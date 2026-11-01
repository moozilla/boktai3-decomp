// MATCHING_CONCESSION: register-pinning
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
        register u32 value asm("r0") = *flags;
        register u32 one asm("r1") = 1;
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
        register u32 value asm("r0") = *flags;
        register u32 mask asm("r1");
        asm("" ::: "r1");
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
        u32 ignored = gEngineFlags & 0x4800;
        u32 counter;
        u32 value;
        asm volatile("" : "+r"(ignored));
        {
            u32 offset = 0x5B2;
            u16 *field;
            field = (void *)((u8 *)state + offset);
            counter = (u32)*field << 16;
        }
        value = counter >> 16;
        if (value <= 19) {
            if (({
                    u32 shifted = counter >> 18;
                    u32 one;
                    one = 1;
                    shifted & one;
                }))
                Show(actor);
            else
                Hide(actor);
        } else if (value <= 29) {
            if (({
                    u32 shifted = counter >> 17;
                    u32 one;
                    one = 1;
                    shifted & one;
                }))
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
