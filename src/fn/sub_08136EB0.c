#include "global.h"
#include "scene_actor_registry.h"
inline static u8 blocked2048(u8 *c)
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

void SceneActorControl_RestoreInactiveDefaults(struct SceneActorRegistryActor *actor)
{
    u32 one;
    u32 flags;
    if (!blocked2048((void *)actor)) {
        one = 1;
        flags = actor->flags2CC;
        one &= flags;
        if (!one && (flags & 2)) {
            SceneActorRegistry_RestoreActorDefaults(actor);
            actor->field178 = one;
        }
    }
}
