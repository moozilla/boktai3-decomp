#include "global.h"
#include "scene_pool.h"
#define ScenePool_CanAcceptControl sub_0811628C
static inline u8 ScenePool_HasControlRestriction(struct ScenePoolActor *actor, u32 mask)
{
    if (actor->flags5ee & mask)
        return 1;
    return 0;
}
static inline u32 ScenePool_TestPlayerFlags(u32 flags, u32 mask)
{
    return flags & mask;
}
u8 sub_0811628C(struct ScenePoolActor *actor)
{
    if ((actor->linkedPlayer && ScenePool_TestPlayerFlags(actor->linkedPlayer->flags, 4)) ||
        ScenePool_HasControlRestriction(actor, 4))
        return 0;
    return 1;
}
