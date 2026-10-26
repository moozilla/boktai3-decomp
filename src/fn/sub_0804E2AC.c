#include "area_effect.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 Region_FindContaining(void *);
struct PackedPoint {
    u32 xy;
    u32 z : 16;
    u32 pad : 16;
};
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
                struct PackedPoint point;
                manager->entries[*count].xMin = Script_GetValue();
                manager->entries[*count].zMin = Script_GetValue();
                manager->entries[*count].xMax = Script_GetValue();
                manager->entries[*count].zMax = Script_GetValue();
                {
                    u32 offset = *count * sizeof(struct AreaEffectEntry);
                    u16 *xMin = (u16 *)((u8 *)manager + offset + 0xCC);
                    u32 mask = 0xFFFF0000;
                    point.xy = *xMin;
                    {
                        u32 z = manager->entries[*count].zMin;
                        ((u32 *)&point)[1] = (((u32 *)&point)[1] & mask) | z;
                    }
                }
                manager->entries[*count].cachedValue = Region_FindContaining(&point);
                manager->entries[*count].enabled = k2;
                (*count)++;
            }
        }
    }
}
