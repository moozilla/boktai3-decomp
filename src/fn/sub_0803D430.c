#include "scene_pool.h"
void ScenePool_SpawnEntriesNearPlayers(struct ScenePoolManager *);
void ScenePool_UpdateOccupiedActorSlots(struct ScenePoolManager *);
// confidence=low: records the position, map region and presence mask of each player (stopping
// at the first missing one), then spawns nearby entries and updates occupied actor slots.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// The ROM does not strength-reduce regions[i] (it recomputes i*4 each iteration) and keeps m in r8.
s32 sub_0803D430(struct ScenePoolManager *m)
{
    s32 i;
    u32 *regions = m->regions;
    do {
        m->playerCount = 0;
    } while (0);
    m->playerMask = 0;
    for (i = 0; i < 2; i++) {
        struct ScenePoolPlayer *player = gUnk_020004F0[i];
        if (player == ((void *)0)) {
            do {
                m->playerPositions[i][0] = 0;
            } while (0);
            m->playerPositions[i][1] = 0;
            m->playerPositions[i][2] = 0;
            break;
        }
        m->playerPositions[i][0] = player->position[0];
        m->playerPositions[i][1] = player->position[1];
        m->playerPositions[i][2] = player->position[2];
        regions[i] = ((u32)m->playerPositions[i][0]) >> 24;
        m->playerMask |= 1 << i;
        m->playerCount++;
    }

    ScenePool_SpawnEntriesNearPlayers(m);
    ScenePool_UpdateOccupiedActorSlots(m);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0803D430);
#endif
