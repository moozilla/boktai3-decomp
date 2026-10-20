#include "global.h"
#include "engine.h"

struct TManager {
    u8 task[0x18];
    u32 activeMask;
    s32 categoryCounts[4], totalCounts[4];
    s32 count;
    u8 pad40[0x108 - 0x40];
    u8 *entries;
};

s32 ResourceRegistry_RegisterResource(u32, void *, u32);
void sub_081DA0B4(void *, void *, s32);
void SpriteGroup_InitFourSprites(void *, u32);

s32 TimedEffect_InitManager(struct TManager *manager, s32 count)
{
    u8 *entry;
    s32 i;
    u8 *heap;

    manager->count = count;
    heap = Heap_Alloc(count * 0x1AC);
    manager->entries = heap;
    if (heap == NULL)
        return -1;
    Memory_Clear(heap, manager->count * 0x1AC);
    ResourceRegistry_RegisterResource(0x4E01, manager, 0);
    manager->activeMask = 0;
    entry = manager->entries;
    for (i = 0; i < manager->count;) {
        sub_081DA0B4(manager, entry, i);
        i++;
        entry += 0x1AC;
    }
    for (i = 0; i < 4; i++) {
        manager->categoryCounts[i] = 0;
        manager->totalCounts[i] = 0;
    }
    SpriteGroup_InitFourSprites(manager, 3);
    return 0;
}
