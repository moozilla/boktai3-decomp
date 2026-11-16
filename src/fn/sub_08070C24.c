#include "scene_actor_registry.h"
#include "player.h"
#include "ram.h"
s32 Path_GetPointPositionById(void *, u32, u32);
static inline void Bounds_Subtract(struct ContactPosition *out, const struct SceneVec *a,
                                   const struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline s32 Bounds_PlanarDistanceSquared(const struct ContactPosition *v)
{
    return v->x * v->x + v->z * v->z;
}
u32 SceneActorRegistry_TestNearestPlayerBounds(struct ChaseActor *actor,
                                               const struct ContactPosition *bounds)
{
    s32 nearestDistance = 0x7fffffff;
    s32 nearestIndex = -1;
    struct ContactPosition origin, difference, bestDifference;
    s32 i;
    u32 category = actor->base.category;
    if (category == 10) {
        u32 mask = 0x4000000;
        if (actor->base.statusFlags & mask)
            return 1;
        return 0;
    }
    Path_GetPointPositionById((u16 *)&origin, actor->group.homeRoute.routeId,
                              actor->group.homeRoute.pointId);
    for (i = 0; i < gPlayerCount; i++) {
        struct Player *player = gPlayers[i];
        if (player) {
            s32 distance;
            Bounds_Subtract(&difference, &player->position, &origin);
            distance = Bounds_PlanarDistanceSquared(&difference);
            if (nearestDistance > distance) {
                nearestDistance = distance;
                nearestIndex = i;
                bestDifference = difference;
            }
        }
    }
    if (nearestIndex >= 0 && bestDifference.x <= bounds->x && bestDifference.x >= -bounds->x &&
        bestDifference.z <= bounds->z && bestDifference.z >= -bounds->z)
        return 1;
    return 0;
}
