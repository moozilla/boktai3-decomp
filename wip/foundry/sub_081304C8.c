#define SceneActorControl_EnterHiddenState sub_081304C8
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
void SceneActorTerrain_DisableCollision(void *);
void sub_0807FC90(void);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline void ShowHidden(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (!actor->drawableKind)
        *(u32 *)actor->drawable |= mask;
    else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
        tile->flags |= mask;
    }
}
static inline void SetFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 offset = 0x2D4;
    u16 *flags = (u16 *)((u8 *)actor + offset);
    u32 value = *flags;
    mask |= value;
    *flags = mask;
}
void sub_081304C8(struct SceneActorRegistryResetView *actor)
{
    if (TakeFlag(&actor->field2B1)) {
        struct ContactPosition *position;
        if (!actor->drawableKind)
            position = (void *)((u8 *)actor->drawable + 0x1C);
        else
            position = (void *)((u8 *)actor->drawable + 0x40);
        position->y = 30000;
    }
    actor->position.x = 0;
    actor->position.y = 30000;
    actor->position.z = 0;
    ShowHidden(actor, 1);
    {
        u32 state = 3;
        u32 stateOffset = 0x2A8;
        u8 *field = (u8 *)actor + stateOffset;
        u32 zero = 0;
        u16 phaseOffset;
        void *control;
        u16 kind;
        *field = state;
        phaseOffset = 0x2A9;
        *((u8 *)actor + phaseOffset) = zero;
        actor->behavior2AA = zero;
        stateOffset += 4;
        *(u32 *)((u32)actor + stateOffset) = zero;
        phaseOffset += 8;
        {
            u8 *ready = (u8 *)actor + phaseOffset;
            u32 one = 1;
            *ready = one;
            control = (void *)sub_0807FC90;
            kind = 16;
            phaseOffset += 1;
            *((u8 *)actor + phaseOffset) = one;
        }
        {
            u32 offset = 0x2B0;
            *((u8 *)actor + offset) = zero;
            phaseOffset += 3;
            *((u8 *)actor + phaseOffset) = kind;
            offset -= 0x94;
            *(void **)((u8 *)actor + offset) = control;
        }
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
    }
    SceneActorTerrain_DisableCollision(actor);
    {
        u32 mask = 0x40;
        SetFlags(actor, mask);
    }
    ++actor->counter2AC;
}
