#include "global.h"
struct SceneEntry {
    u8 unknown0[0xB8];
};
struct SceneEntryPool {
    u8 unknown0[0x1D];
    u8 occupied;
    u8 unknown1E[0x18C - 0x1E];
    struct SceneEntry entries[4];
};
void sub_08198F3C(struct SceneEntryPool *, struct SceneEntry *, void *, void *);
void SceneEntryPool_CreateEntry(struct SceneEntryPool *pool, void *arg1, void *arg2)
{
    struct SceneEntry *entry = 0;
    s32 index;
    for (index = 0; index < 4; index++) {
        if (!((pool->occupied >> index) & 1)) {
            entry = &pool->entries[index];
            pool->occupied |= 1 << index;
            break;
        }
    }
    if (entry)
        sub_08198F3C(pool, entry, arg1, arg2);
}
