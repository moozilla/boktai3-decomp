#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"

static inline void SubtractPosition(s16 *delta, const struct SceneVec *a,
                                    const struct ContactPosition *b)
{
    delta[0] = a->x - b->x;
    delta[1] = a->y - b->y;
    delta[2] = a->z - b->z;
}
void *SceneActorTarget_FindNearbyPlayer(struct SceneActorRegistryResetView *actor)
{
    void *result = 0;
    s32 limit = actor->distanceLimitF8 * 2;

    u32 flags;
    s16 delta[4];
    struct Player *volatile *players;
    struct ContactPosition *position;
    s32 count;
    u32 excluded;
    s16 *difference;
    {
        struct Player *volatile *g0 = (struct Player **)0x02000580;
        struct Player *initial = g0[0];
        u32 mask = 0x10000;
        flags = initial->statusFlags;
        if (flags & mask) {
            limit = 0x24000;
        }
    }
    if ((actor->field2B5 == 3) && (flags & 0x40000)) {
        return 0;
    }
    excluded = 0x1000;
    difference = delta;
    position = &actor->position;
    players = gPlayers;
    count = 0;
    do {
        struct Player *player = *players;
        if (player && !(player->statusFlags & excluded)) {
            s32 distance;
            SubtractPosition(difference, &player->position, position);
            distance = difference[0] * difference[0] + difference[2] * difference[2];
            if (limit > distance) {
                limit = distance;
                result = (u8 *)*players + 0x24;
            }
        }
        players++;
        count--;
    } while (count >= 0);
    return result;
}
