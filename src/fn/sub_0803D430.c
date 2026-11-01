#include "scene_pool.h"
void ScenePool_SpawnEntriesNearPlayers(struct ScenePoolManager *);
void ScenePool_UpdateOccupiedActorSlots(struct ScenePoolManager *);

#ifdef NONMATCHING
// fuzzy: 85.9%
// confidence=low: records active player positions, then updates nearby pooled actors.
s32 sub_0803D430(struct ScenePoolManager *m)
{
    s32 i;
    u32 *regions;
    m->playerCount = 0;
    m->playerMask = 0;
    regions = m->regions;
    for (i = 0; i < 2; i++) {
        struct ScenePoolPlayer *player = gParticipantActors[i];
        u32 offset = (u32)i * sizeof(*regions);
        if (!player) {
            m->playerPositions[i][0] = 0;
            m->playerPositions[i][1] = 0;
            m->playerPositions[i][2] = 0;
            break;
        }
        m->playerPositions[i][0] = player->position[0];
        m->playerPositions[i][1] = player->position[1];
        m->playerPositions[i][2] = player->position[2];
        {
            u32 region = (u32)m->playerPositions[i][0] >> 24;
            regions[offset / sizeof(*regions)] = region;
        }
        m->playerMask |= 1u << i;
        m->playerCount++;
    }
    ScenePool_SpawnEntriesNearPlayers(m);
    ScenePool_UpdateOccupiedActorSlots(m);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0803D430);
#endif
