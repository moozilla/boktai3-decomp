#include "scene_actor_availability.h"
#include "scene_actor_control_helpers.h"

static inline void Hide(struct SceneActorRegistryResetView *actor)
{
    u32 one = 1;
    if (actor->drawableKind == 0) {
        u32 *flags = actor->drawable;
        *flags |= one;
    } else {
        struct Tile {
            u8 unknown0[8];
            u32 flags;
        };
        struct Tile *tile = (void *)((u8 *)actor->drawable + 0x20);
        tile->flags |= one;
    }
    {
        u32 offset = 0x320;
        ActorShadow_Disable((void *)((u8 *)actor + offset));
    }
}

static inline void Show(struct SceneActorRegistryResetView *actor)
{
    if (actor->drawableKind == 0) {
        u32 *flags = actor->drawable;
        u32 mask;
        mask = ~1;
        *flags &= mask;
    } else {
        struct Tile {
            u8 unknown0[8];
            u32 flags;
        };
        struct Tile *tile = (void *)((u8 *)actor->drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
    {
        u32 offset = 0x320;
        ActorShadow_Enable((void *)((u8 *)actor + offset));
    }
}
void SceneActorControl_TickVisibility(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state;
    u32 unavailable = SceneActor_ActionsBlocked((void *)actor);
    if (unavailable)
        return;
    {
        u32 offset = 0x3D0;
        u32 mask;
        u32 active;
        state = SceneActorControl_GetMotionState(actor);
        mask = 0x60;
        active = SceneActorControl_TestFlags(state->flags3D4, mask);
        if (!active)
            return;
    }
    {
        u32 counter;
        u32 value;
        {
            u16 *field = &state->visibilityFrame;
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
            inputFlags = &state->flags3D4;
            active = *inputFlags & mask;
            flags = inputFlags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (active) {
                if (actor->drawableKind == 0) {
                    u32 *field = actor->drawable;
                    u32 mask = ~1;
                    *field &= mask;
                } else {
                    struct Tile {
                        u8 unknown0[8];
                        u32 flags;
                    };
                    struct Tile *tile = (void *)((u8 *)actor->drawable + 0x20);
                    {
                        u32 mask = ~1;
                        tile->flags &= mask;
                    }
                }
                {
                    u32 mask = ~0x20;
                    *flags &= mask;
                }
                ActorShadow_Enable((void *)((u8 *)actor + 0x320));
            } else {
                u32 one = 1;
                if (actor->drawableKind == 0) {
                    u32 *field = actor->drawable;
                    *field |= one;
                } else {
                    struct Tile {
                        u8 unknown0[8];
                        u32 flags;
                    };
                    struct Tile *tile = (void *)((u8 *)actor->drawable + 0x20);
                    tile->flags |= one;
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
        u16 *counter = &state->visibilityFrame;
        (*counter)++;
    }
}
