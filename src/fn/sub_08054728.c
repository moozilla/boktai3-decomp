#include "hit_switch.h"
void SceneBody_Unregister(struct SceneBody *);
void TerrainModifiers_UnregisterCell(struct SceneCellRef *);
s32 HitSwitch_DestroyManager(struct HitSwitchManager *manager)
{
    s32 i;
    for (i = 0; i < manager->count; i++) {
        struct HitSwitchEntry *entry = &manager->entries[i];
        if (entry->bodyEnabled)
            SceneBody_Unregister(&entry->body);
        Sprite_Remove(&entry->sprite);
        Collider_Remove(&entry->collider);
        if (entry->cellRegistered)
            TerrainModifiers_UnregisterCell(&entry->cellRef);
    }
    gHitSwitchManager = 0;
    return 0;
}
