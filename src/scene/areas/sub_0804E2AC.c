#include "scene/areas/area_effect.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 Region_FindContaining(void *);
void AreaEffect_AddRectangleFromScript(void)
{
    struct AreaEffectManager *manager = gAreaEffectManager;
    s32 k2 = 1;
    if (manager) {
        u8 *cursor = &manager->count;
        u32 n = *cursor;
        u8 *count = cursor;
        if (n <= 15) {
            u32 found = Script_SeekToKeyword(0x6E);
            if (found) {
                manager->entries[*count].tag = Script_GetValue();
            } else {
                manager->entries[*count].tag = found;
            }
            if (Script_SeekToKeyword(0x61)) {
                struct ContactPosition point;
                manager->entries[*count].xMin = Script_GetValue();
                manager->entries[*count].zMin = Script_GetValue();
                manager->entries[*count].xMax = Script_GetValue();
                manager->entries[*count].zMax = Script_GetValue();
                point.x = manager->entries[*count].xMin;
                point.y = 0;
                point.z = manager->entries[*count].zMin;
                manager->entries[*count].cachedValue = Region_FindContaining(&point);
                manager->entries[*count].enabled = k2;
                (*count)++;
            }
        }
    }
}
