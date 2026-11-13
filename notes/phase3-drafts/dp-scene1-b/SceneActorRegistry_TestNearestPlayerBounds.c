// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#include "scene_actor_state.h"
#include "player.h"
#include "ram.h"
s32 Path_GetPointPositionById(u16 *, u32, u32);
u32 SceneActorRegistry_TestNearestPlayerBounds(struct SceneActorRegistryActor *input,
                                               const struct ContactPosition *inputBounds)
{
    struct SceneActorRegistryResetView *actor = (void *)input;
    register const struct ContactPosition *bounds = inputBounds;
    s32 nearestDistance = 0x7fffffff;
    s32 nearestIndex = -1;
    /* Only x/y/z participate. The fourth halfword remains unspecified through
  * aggregate copies and is never read as a scalar. */
    struct ContactPosition origin, difference, bestDifference;
    if (actor->category == 10) {
        u32 mask = 0x4000000;
        u32 value = actor->flags2D0;
        value &= mask;
        if (value)
            goto success;
        goto failure;
    }
    {
        struct SceneActorRegistryRoute *route = &actor->group.homeRoute;
        Path_GetPointPositionById((u16 *)&origin, route->routeId, route->pointId);
    }
    {
        s32 index = 0;
        s32 initialCount = gPlayerCount;
        if (index < initialCount) {
            s32 count;
            struct ContactPosition *delta = &difference;
            struct ContactPosition *left = &origin;
            register struct Player **playerSlot;
            count = initialCount;
            {
                register struct Player **initial = gPlayers;
                playerSlot = initial;
            }
            do {
                register struct Player **current = playerSlot;
                register struct Player *player;
                player = *current;
                if (player) {
                    const struct SceneVec *right = &player->position;
                    s32 x;
                    s32 z;
                    s32 xSquare;
                    s32 zSquare;
                    s32 distance;
                    delta->x = (u16)player->position.x - (u16)left->x;
                    delta->y = (u16)right->y - (u16)left->y;
                    delta->z = (u16)right->z - (u16)left->z;
                    x = delta->x;
                    xSquare = x * x;
                    z = delta->z;
                    zSquare = z * z;
                    {
                        s32 copy = zSquare;
                        distance = (s32)((u32)xSquare + (u32)copy);
                    }
                    if (nearestDistance > distance) {
                        nearestDistance = distance;
                        nearestIndex = index;
                        bestDifference = difference;
                    }
                }
                playerSlot++;
                index++;
            } while (index < count);
        }
    }
    if (nearestIndex < 0)
        goto failure;
    if (bestDifference.x > bounds->x || bestDifference.x < -bounds->x ||
        bestDifference.z > bounds->z || bestDifference.z < -bounds->z)
        goto failure;

success:
    return 1;
failure:
    return 0;
}