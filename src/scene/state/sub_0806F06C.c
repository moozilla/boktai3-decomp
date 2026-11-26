#include "scene/actor/scene_actor_registry.h"
#include "scene/actor/scene_actor_status.h"
#include "scene/actor/scene_actor_components.h"
struct ChaseActor *SceneActorRegistry_FindAvailableAfterId(u32);
struct ChaseActor *sub_0806F06C(const struct ContactPosition *center,
                                const struct ContactPosition *radius, u32 *cursor)
{
    s32 i = 0;
    do {
    repeat: {
        struct ChaseActor *a = SceneActorRegistry_FindAvailableAfterId(*cursor);
        if (!a) {
            return 0;
        }
        {
            if (ActorStatus_HasStatusFlags(a, 0x400)) {
                continue;
            }
        }
        *cursor = a->base.body.id;
        {
            struct SceneActorBody *body = &a->base.body;
            if (ActorStatus_IsDown(a)) {
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
