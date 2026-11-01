#include "scene_pool.h"
void ScenePool_SpawnEntriesNearPlayers(struct ScenePoolManager *);
void ScenePool_UpdateOccupiedActorSlots(struct ScenePoolManager *);

#ifdef NONMATCHING
// fuzzy: 86.2%
// confidence=low: records active player positions, then updates nearby pooled actors.
// Order and shape match (source-level pointers set before the counter; the null exit is a goto so
// stmt.c does not roll it into a duplicated exit test; `offset & ~3` keeps loop.c from reducing
// the region address and keeps regions first). Left: global alloc ranks offset (4 refs, live 13)
// above i (11 refs, live 30 doubled by update_equiv_regs for its constant first set), so i takes
// r5; and reload picks r1 for the regions address where the ROM's r1 is unavailable (spill set
// {r0, r3}). AGBCC_PRI=i:+654 AGBCC_SPILL=0:2 leaves 2 insns.
s32 ScenePool_UpdateManager(struct ScenePoolManager *m)
{
    s32 i;
    u32 *regions;
    u32 *mask;
    s32 *pos;
    s32 *count;
    struct ScenePoolPlayer **pp;
    m->playerCount = 0;
    m->playerMask = 0;
    regions = m->regions;
    mask = &m->playerMask;
    pos = m->playerPositions[0];
    count = &m->playerCount;
    pp = gParticipantActors;
    for (i = 0; i < 2; i++) {
        struct ScenePoolPlayer *player;
        u32 offset = (u32)i * sizeof(*regions);
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
        *mask |= 1u << i;
        (*count)++;
        pos += 4;
        pp++;
    }
done:
    ScenePool_SpawnEntriesNearPlayers(m);
    ScenePool_UpdateOccupiedActorSlots(m);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", ScenePool_UpdateManager);
#endif
