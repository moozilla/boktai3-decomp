#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"

void *SceneActor_FindNearbyPlayerTarget(struct SceneActorRegistryResetView *actor)
{
    void *target = 0;
    s32 limit = actor->distanceLimitF8 * 2;
    struct ContactPosition difference;
    struct Player **slot;
    s32 i;
    u32 flags;
    {
        struct Player **g0 = (struct Player **)0x02000580;
        struct Player *player = g0[0];
        u32 mask = 0x10000;
        flags = player->statusFlags;
        if (mask & flags) {
            limit = 0x24000;
        }
    }
    if ((actor->field2B5 == 3) && (flags & 0x40000)) {
        return 0;
    }
    {
        u32 blocked = 0x1000;
        struct ContactPosition *delta = &difference;
        const struct ContactPosition *origin = &actor->position;
        {
            u32 address = (u32)gPlayers;
            slot = (struct Player **)address;
        }
        i = 0;
        do {
            struct Player *player = *slot;
            if (player && !(player->statusFlags & blocked) && player->positionResetValue == 0) {
                s32 distance;
                const struct SceneVec *position = &player->position;
                delta->x = position->x - origin->x;
                delta->y = position->y - origin->y;
                delta->z = position->z - origin->z;
                distance = delta->x * delta->x + delta->z * delta->z;
                if (limit > distance) {
                    limit = distance;
                    target = &(*slot)->positionalEffectGroup;
                }
            }
            slot++;
            i--;
        } while (i >= 0);
    }
    return target;
}
