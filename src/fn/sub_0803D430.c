#include "scene_pool.h"
void ScenePool_SpawnEntriesNearPlayers(struct ScenePoolManager *);
void ScenePool_UpdateOccupiedActorSlots(struct ScenePoolManager *);

/* The scan starts at player zero and addresses the region array through the low halfword. */
union ScenePoolScanWord {
    u32 value;
    struct {
        u16 regionOffset, firstPlayer;
    } scan;
};

// confidence=low: records active player positions, then updates nearby pooled actors.
s32 ScenePool_UpdateManager(struct ScenePoolManager *m)
{
    s32 playerIndex;
    union ScenePoolScanWord word;
    u32 *regions;
    u32 *mask;
    s32 *pos;
    s32 *count;
    struct ScenePoolPlayer **pp;
    word.scan.regionOffset = word.scan.firstPlayer = 0;
    m->playerCount = word.scan.regionOffset;
    m->playerMask = word.scan.regionOffset;
    word.scan.regionOffset = (u8 *)m->regions - (u8 *)m;

    regions = (u32 *)((u8 *)m + word.value);
    mask = &m->playerMask;
    pos = m->playerPositions[0];
    count = &m->playerCount;
    pp = gParticipantActors;
    playerIndex = word.scan.firstPlayer;
    do {
        struct ScenePoolPlayer *player;
        u32 offset = (u32)playerIndex * sizeof(*regions);
        player = *pp;
        if (!player) {
            pos[0] = 0;
            pos[1] = 0;
            pos[2] = 0;
            goto done;
        }
        pos[0] = player->position[0];
        pos[1] = player->position[1];
        pos[2] = player->position[2];
        {
            u32 region = (u32)pos[0] >> 24;
            *(u32 *)((u8 *)regions + (offset & ~3)) = region;
        }
        *mask |= 1u << playerIndex;
        (*count)++;
        pos += 4;
        pp++;
    } while (++playerIndex < 2);
done:
    ScenePool_SpawnEntriesNearPlayers(m);
    ScenePool_UpdateOccupiedActorSlots(m);
    return 0;
}
