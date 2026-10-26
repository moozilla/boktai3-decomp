#include "global.h"
#include "ram.h"

// confidence=low: HitSwitch_DestroyManager for the growth hazard's entries (0x118 bytes each,
// the cell always registered).
struct GrowthEntry {
    u8 sprite[0x48];
    u8 collider[0x9c - 0x48];
    u8 body[0xed - 0x9c];
    u8 bodyEnabled;
    u8 unknownEE[0x104 - 0xee];
    u8 cellRef[0x118 - 0x104];
};
struct GrowthManager {
    u8 unknown0[0x18];
    u8 count;
    u8 unknown19[0x5c - 0x19];
    struct GrowthEntry entries[1];
};
void SceneBody_Unregister(void *);
void Sprite_Remove(void *);
void Collider_Remove(void *);
void TerrainModifiers_UnregisterCell(void *);
#define GrowthManager_Destroy sub_081B7D44
s32 GrowthManager_Destroy(struct GrowthManager *manager)
{
    s32 i;
    for (i = 0; i < manager->count; i++) {
        struct GrowthEntry *entry = &manager->entries[i];
        if (entry->bodyEnabled)
            SceneBody_Unregister(entry->body);
        Sprite_Remove(entry->sprite);
        Collider_Remove(entry->collider);
        TerrainModifiers_UnregisterCell(entry->cellRef);
    }
    gGrowthHazardManager = 0;
    return 0;
}
