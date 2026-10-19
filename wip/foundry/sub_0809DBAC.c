#define RouteActor_BeginReaction sub_0809DBAC
#include "global.h"
#include "scene_actor_reset.h"
static inline u16 SetFlag(u32 mask, struct SceneActorRegistryResetView *actor)
{
    u16 *flags = &actor->flags2D4;
    mask |= *flags;
    return *flags = mask;
}
u32 sub_0809DBAC(struct SceneActorRegistryResetView *actor)
{
    if (!actor->reactionSoundVariant)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    if (actor->field2DC & 0x40000)
        SetFlag(0x20, actor);
    {
        u16 offset = 0x230;
        void *callback = *(void **)((u8 *)actor + offset);
        u32 kind = 18;
        u8 *prepared = &actor->field2B2;
        u32 zero;
        u8 byteZero;
        u8 one;
        zero = 0;
        one = 1;
        *prepared = one;
        offset += 0x80;
        *((u8 *)actor + offset) = zero;
        offset += 5;
        *((u8 *)actor + offset) = kind;
        actor->target = callback;
        {
            u16 *field = &actor->field2C8;
            byteZero = 0;
            *field = zero;
        }
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
        {
            u32 state = 3;
            u32 stateOffset = 0x2A8;
            u32 phaseOffset;
            u32 counterOffset;
            *((u8 *)actor + stateOffset) = state;
            phaseOffset = 0x2A9;
            *((u8 *)actor + phaseOffset) = byteZero;
            stateOffset += 2;
            *((u8 *)actor + stateOffset) = byteZero;
            counterOffset = 0x2AC;
            *(u32 *)((u8 *)actor + counterOffset) = zero;
            phaseOffset += 8;
            *((u8 *)actor + phaseOffset) = one;
            {
                u32 *flags = &actor->flags2D0;
                u32 mask = ~0x1000000;
                *flags &= mask;
            }
            counterOffset += 0x2A;
            {
                u16 *flags = (void *)((u8 *)actor + counterOffset);
                u32 mask = 0xFFFFF636;
                mask &= *flags;
                *flags = mask;
            }
        }
    }
    return 1;
}
