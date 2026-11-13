#include "scene_actor_registry.h"
#include "scene_actor_components.h"
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
struct SceneActorRegistryActor *sub_0806F06C(const struct ContactPosition *center,
                                             const struct ContactPosition *radius, u32 *cursor)
{
    s32 i = 0;
    do {
    repeat: {
        struct SceneActorRegistryActor *a = SceneActorRegistry_FindAvailableAfterId(*cursor);
        if (!a) {
            return 0;
        }
        {
            u32 mask = 0x400;
            if (a->flags2D0 & mask) {
                continue;
            }
        }
        *cursor = a->id;
        {
            struct SceneActorBody *body = (struct SceneActorBody *)&a->id;
            if (Inactive(a)) {
                continue;
            }
            if (center->x - radius->x > body->position.x ||
                ((center->x + radius->x) < body->position.x)) {
                if (++i <= 19)
                    goto repeat;
                return 0;
            }
            if (center->y - radius->y > body->position.y ||
                ((center->y + radius->y) < body->position.y)) {
                continue;
            }
            if (center->z - radius->z > body->position.z ||
                ((center->z + radius->z) < body->position.z)) {
                continue;
            }
            return a;
        }
    }
    } while (++i <= 19);
    return 0;
}
