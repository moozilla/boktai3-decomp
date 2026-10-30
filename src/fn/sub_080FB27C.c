#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
inline static u8 Actor_IsInactive(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

s32 SceneActorControl_CanAdvanceHover(struct SceneActorRegistryResetView *actor)
{
    if (!Actor_IsInactive((void *)actor)) {
        u8 *context = actor->context;
        *(struct ContactPosition *)(context + 0xF1C) =
            *(struct ContactPosition *)((*(u8 **)gPlayers) + 0x30);
        {
            u32 mask = 0x100;
            if (!(actor->flags2D4 & mask) && actor->tracking.distance < actor->tracking.limit)
                goto one;
        }
    }
    return 0;
one:
    return 1;
}
