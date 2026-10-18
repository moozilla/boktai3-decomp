#include "scene_actor_registry.h"
/* SceneActorQuery_FindInRectangle: Advances the actor ID cursor through up to twenty candidates and returns an eligible living actor within inclusive X/Z bounds */
#define SceneActorQuery_FindInRectangle sub_0806EFB8
#define SceneActorQuery_FindInRectangle sub_0806EFB8
struct SceneActorRegistryActor *SceneActorRegistry_FindAvailableAfterId(u32);
static inline u8 Inactive(struct SceneActorRegistryActor *a)
{
    {
        u32 mask = 0x40;
        if (a->flags2D4 & mask)
            return 1;
    }
    {
        u32 mask = 0x20000;
        if (a->flags2D0 & mask)
            return 1;
    }
    if (a->health <= 0)
        return 1;
    return 0;
}
struct SceneActorRegistryActor *
SceneActorQuery_FindInRectangle(const struct ContactPosition *minimum,
                                const struct ContactPosition *size, u32 *cursor)
{
    s32 i = 0;
    do {
        struct SceneActorRegistryActor *a = SceneActorRegistry_FindAvailableAfterId(*cursor);
        if (!a)
            return 0;
        *cursor = a->id;
        {
            u8 *base = (u8 *)a + 0x50;
            if (Inactive(a))
                continue;
            if (minimum->x > *(s16 *)(base + 12) || minimum->x + size->x < *(s16 *)(base + 12))
                continue;
            if (minimum->z > *(s16 *)(base + 16) || minimum->z + size->z < *(s16 *)(base + 16))
                continue;
            return a;
        }
    } while (++i <= 19);
    return 0;
}
