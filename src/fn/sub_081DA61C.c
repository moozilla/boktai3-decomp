#include "global.h"
#define TimedEffect_DestroyManager sub_081DA61C
#define TimedEffect_RemoveFromManager sub_081DA178
#define ResourceSlot_ClearFlags ResourceRegistry_UnregisterResource
struct TimedEffect { u8 unk00[0x1AC]; };
struct ManagerEffect { u8 unk00[0x30]; };
struct TimedEffectManager { u8 unk00[0x18]; u32 activeMask; u8 unk1C[0x3C - 0x1C]; s32 capacity; u8 unk40[8]; struct ManagerEffect effects[4]; struct TimedEffect *entries; };
void Effect_Remove(void *);
void Heap_Free(void *);
void sub_081DA178(struct TimedEffectManager *, struct TimedEffect *, u32);
void ResourceRegistry_UnregisterResource(u16);
s32 sub_081DA61C(struct TimedEffectManager *manager)
{
    struct ManagerEffect *effect = manager->effects;
    s32 i = 3;
    struct TimedEffect *entry;
    do {
        Effect_Remove(effect);
        effect++;
    } while (--i >= 0);
    entry = manager->entries;
    for (i = 0; i < manager->capacity; i++, entry++) {
        if (manager->activeMask & (1 << i))
            sub_081DA178(manager, entry, i);
    }
    Heap_Free(manager->entries);
    ResourceRegistry_UnregisterResource(0x4E01);
    return 0;
}
