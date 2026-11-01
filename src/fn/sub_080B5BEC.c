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
    register struct SceneActorRegistryResetView *actor asm("r3") = input;
    if (actor) {
        if (ReadFlags16(actor, 0x40) || ReadFlags32(actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}
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

static inline void Show(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    if (actor->drawableKind == 0) {
        u32 *flags = actor->drawable;
        u32 mask;
        asm("" ::: "r1");
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
void sub_080B5BEC(struct SceneActorRegistryResetView *input)
{
    register struct SceneActorRegistryResetView *actor asm("r3") = input;
    struct SceneActorControlMotionState *state;
    u32 unavailable = IsUnavailable(actor);
    if (unavailable)
        return;
    {
        u32 offset = 0x3D0;
        u32 mask;
        u32 active;
        state = *(void **)((u8 *)actor + offset);
        mask = 0xC000;
        offset = 0x6DC;
        active = SceneActorControl_TestFlags(*(u32 *)((u8 *)state + offset), mask);
        if (!active)
            return;
    }
    {
        u32 counter;
        u32 value;
        {
            u32 offset = 0x748;
            u16 *field;
            field = (void *)((u8 *)state + offset);
            counter = (u32)*field << 16;
        }
        value = counter >> 16;
        if (value <= 19) {
            if (({
                    u32 shifted = counter >> 18;
                    register u32 one asm("r1");
                    one = 1;
                    shifted & one;
                }))
                Show(actor);
            else
                Hide(actor);
        } else if (value <= 29) {
            if (({
                    u32 shifted = counter >> 17;
                    register u32 one asm("r1");
                    one = 1;
                    shifted & one;
                }))
                Show(actor);
            else
                Hide(actor);
        } else {
            u32 mask = 0x4000;
            u32 *inputFlags;
            u32 *flags;
            u32 active;
            inputFlags = (u32 *)((u8 *)state + 0x6DC);
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
                    u32 mask = ~0x4000;
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
                    u32 mask = ~0x8000;
                    *flags &= mask;
                }
                ActorShadow_Disable((void *)((u8 *)actor + 0x320));
            }
        }
    }
    {
        u16 *counter = (void *)((u8 *)state + 0x748);
        (*counter)++;
    }
}
