// MATCHING_CONCESSION: register-pinning
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
#ifdef NONMATCHING
// fuzzy: 96.7%
// fix.py best (sweep after the inlined-insn fix)
void ScenePool_SpawnEntriesNearPlayers(struct ScenePoolManager *m0)
{
    register struct ScenePoolManager *m asm("r8") = m0;
    register s32 i asm("sl");
    s32 next;
    u32 regions[2];
    m->firstId = 0;
    m->lastId = 0;
    i = 0;
    if (i < m->playerCount) {
        u32 *right = m->rightBounds;
        u32 *left = m->leftBounds;
        do {
            ScenePool_ReadActorBounds(i, left, right, regions);
            right++;
            left++;
            i++;
        } while (i < m->playerCount);
    }
    if (m->currentConfiguration == m->configuration) {
        if (PlayerHasFlag(5))
            return;
    }
    for (next = 0; (i = next) < m->playerCount;) {
        s32 k;
        s32 n;
        s32 nn;
        n = (regions[0] == regions[1]) ? (1) : (2);
        k = 0;
        next = i + 1;
        nn = n;
        while (k < n) {
            struct ScenePoolNode *node =
                ((struct ScenePoolNode **)m->registry[1])[sub_08249498(regions[k], m->registry[0])];
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
                                {
                                    u32 *fp = &m->firstId;
                                    if ((*fp) > e->index)
                                        *fp = e->index;
                                }
                                {
                                    u32 *lp = &m->lastId;
                                    if ((*lp) < e->index)
                                        *lp = e->index;
                                }
                            }
                        }
                    }
                    node = node->next;
                } while (node);
            }
            k++;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", ScenePool_SpawnEntriesNearPlayers);
#endif
