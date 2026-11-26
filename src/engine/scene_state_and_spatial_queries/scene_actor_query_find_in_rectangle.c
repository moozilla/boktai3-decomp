#include "engine/shared/scene_actor_registry.h"
#include "engine/shared/scene_actor_status.h"
/* SceneActorQuery_FindInRectangle: Advances the actor ID cursor through up to twenty candidates and returns an eligible living actor within inclusive X/Z bounds */
struct ChaseActor *SceneActorRegistry_FindAvailableAfterId(u32);
struct ChaseActor *SceneActorQuery_FindInRectangle(const struct ContactPosition *minimum,
                                                   const struct ContactPosition *size, u32 *cursor)
{
    s32 i = 0;
    do {
        struct ChaseActor *a = SceneActorRegistry_FindAvailableAfterId(*cursor);
        if (!a)
            return 0;
        *cursor = a->base.body.id;
        {
            struct SceneActorBody *body = &a->base.body;
            if (ActorStatus_IsDown(a))
                continue;
            if (minimum->x > body->position.x || minimum->x + size->x < body->position.x)
                continue;
            if (minimum->z > body->position.z || minimum->z + size->z < body->position.z)
                continue;
            return a;
        }
    } while (++i <= 19);
    return 0;
}
