#include "global.h"
/* SceneActorControl_BeginSoundTransition_080EBC38: SceneActorControl_BeginSoundTransition with other constants (ported by relatives.py + repair.py; provisional name) */
#include "scene_actor_control_phase.h"
#include "engine.h"
void sub_080EAC7C(struct SceneActorRegistryResetView *, s32);
static inline void BeginTransition(struct SceneActorRegistryResetView *actor, u16 zero, u32 offset)
{
    u32 state = 3;
    u32 stateOffset = 0x2A8;
    u8 *field = (u8 *)actor + stateOffset;
    *field = state;
    offset -= 0x2B;
    *((u8 *)actor + offset) = zero;
    actor->behavior2AA = zero;
    stateOffset += 4;
    *(u32 *)((u8 *)actor + stateOffset) = zero;
    offset += 8;
    *((u8 *)actor + offset) = 1;
    {
        void *callback = sub_080EAC7C;
        u32 kind = 0x2A;
        offset++;
        *((u8 *)actor + offset) = 1;
        stateOffset = 0x2B0;
        *((u8 *)actor + stateOffset) = zero;
        offset += 3;
        *((u8 *)actor + offset) = kind;
        stateOffset -= 0x94;
        *(void **)((u8 *)actor + stateOffset) = callback;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
    }
}
s32 SceneActorControl_BeginSoundTransition_080EBC38(struct SceneActorRegistryResetView *actor)
{
    u16 *flags;
    u16 suppressed;
    u32 offset;
    u32 value;
    s32 result;
    if (actor->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    {
        u32 mask = 0x100;
        offset = 0x2D4;
        flags = (void *)((u8 *)actor + offset);
        value = *flags;
        suppressed = mask & value;
    }
    if (!suppressed) {
        BeginTransition(actor, suppressed, offset);
        result = 1;
    } else {
        u32 mask = ~0x20;
        mask &= value;
        *flags = mask;
        result = 0;
    }
    return result;
}
