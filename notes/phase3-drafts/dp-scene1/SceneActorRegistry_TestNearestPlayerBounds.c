#include "scene_actor_registry.h"
#include "player.h"
#include "ram.h"
s32 Path_GetPointPositionById(u16 *, u32, u32);
struct SceneActorBoundsRouteView {
    u8 unknown0[0x458];
    u8 pathId, unknown459, pointId;
};
u32 SceneActorRegistry_TestNearestPlayerBounds(struct SceneActorRegistryActor *input,
                                               const struct ContactPosition *inputBounds)
{
    struct SceneActorRegistryActor *actor = input;
    const struct ContactPosition *bounds = inputBounds;
    s32 nearestDistance = 0x7fffffff;
    s32 nearestIndex = -1;
    /* Only x/y/z participate. The fourth halfword remains unspecified through
  * aggregate copies and is never read as a scalar. */
    struct ContactPosition origin, difference, bestDifference;
    if (actor->category == 10) {
        u32 mask = 0x4000000;
        u32 *field;
        u32 value;
        field = &actor->flags2D0;
        value = *field;
        value &= mask;
        if (value)
            goto success;
        goto failure;
    }
    {
        struct SceneActorBoundsRouteView *route = (void *)actor;
        u32 path = route->pathId;
        u32 point = route->pointId;
        Path_GetPointPositionById((u16 *)&origin, path, point);
    }
    {
        s32 index = 0;
        s32 initialCount = gPlayerCount;
        if (index < initialCount) {
            s32 count;
            struct ContactPosition *delta = &difference;
            struct ContactPosition *left = &origin;
            struct Player **playerSlot;

            count = initialCount;
            {
                struct Player **initial = gPlayers;

                playerSlot = initial;
            }
        nextPlayer: {
                struct Player **current = playerSlot;
                struct Player *player;
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
                if (index < count)
                    goto nextPlayer;
            }
        }
    }
    if (nearestIndex < 0)
        goto failure;
    {
        const struct ContactPosition *check;
        s32 component;
        s32 limit;
        {
            u32 wide = (u32)(u16)bestDifference.x << 16;
            component = (s32)wide >> 16;
        }
        check = bounds;
        limit = check->x;
        if (component > limit || component < -limit)
            goto failure;
        {
            u32 wide = (u32)(u16)bestDifference.z << 16;
            component = (s32)wide >> 16;
        }
        limit = check->z;
        if (component > limit || component < -limit)
            goto failure;
    }
success:
    return 1;
failure:
    return 0;
}