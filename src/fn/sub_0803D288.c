#include "scene_pool.h"
struct ScenePoolNode {
    u32 unknown0;
    struct ScenePoolEntry *entry;
    struct ScenePoolNode *next;
};
s32 Mod(s32, s32);
u32 sub_08249498(u32, u32);
void ScenePool_ReadActorBounds(s32, u32 *, u32 *, u32 *);
s32 ScenePool_IsEntryInSpawnBand(void *, s32, struct ScenePoolEntry *);
void *ScenePool_FindFreeActorSlot(void *);
void sub_0803BC88(void *, struct ScenePoolEntry *, u32);
static inline u8 PlayerHasFlag(u32 mask)
{
    if (gParticipantActors[0] && (gParticipantActors[0]->flags & mask))
        return 1;
    return 0;
}

void ScenePool_SpawnEntriesNearPlayers(struct ScenePoolManager *m)
{
    s32 i;
    u32 regions[2];
    m->firstId = 0;
    m->lastId = 0;
    for (i = 0; i < m->playerCount; i++)
        ScenePool_ReadActorBounds(i, &m->leftBounds[i], &m->rightBounds[i], regions);
    if (m->currentConfiguration == m->configuration && PlayerHasFlag(5))
        return;
    for (i = 0; i < m->playerCount; i++) {
        s32 k;
        s32 n;
        n = regions[0] == regions[1] ? 1 : 2;
        for (k = 0; k < n; k++) {
            u32 *region = &regions[k];
            u32 *bucketCount = &m->registry[0];
            u32 hash = sub_08249498(*region, *bucketCount);
            struct ScenePoolNode *node = ((struct ScenePoolNode **)m->registry[1])[hash];
            if (node) {
                u32 *seen = m->seenIds;
                do {
                    struct ScenePoolEntry *e = node->entry;
                    u32 idx = e->index;
                    if (!(seen[idx >> 5] & (1 << Mod(idx, 0x20)))) {
                        if (ScenePool_IsEntryInSpawnBand(m, i, e)) {
                            void *slot = ScenePool_FindFreeActorSlot(m);
                            if (slot) {
                                u32 idx2 = e->index;
                                seen[idx2 >> 5] |= 1 << Mod(idx2, 0x20);
                                sub_0803BC88(slot, e, e->index);
                                if (m->firstId > e->index)
                                    m->firstId = e->index;
                                if (m->lastId < e->index)
                                    m->lastId = e->index;
                            }
                        }
                    }
                    node = node->next;
                } while (node);
            }
        }
    }
}
