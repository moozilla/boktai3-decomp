#include "area_effect.h"
// MATCHING_CONCESSION: register-pinning
#define AreaEffect_AddRectangleFromScript sub_0804E2AC
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 sub_0822C034(void *);
struct PackedPoint {
    u32 xy;
    u32 z : 16;
    u32 pad : 16;
};
void AreaEffect_AddRectangleFromScript(void)
{
    struct AreaEffectManager *manager = gUnk_0200010C;
    if (manager) {
        register u8 *cursor asm("r0") = &manager->count;
        register u32 n asm("r1") = *cursor;
        u8 *count = cursor;
        if (n <= 15) {
            u32 found = Script_SeekToKeyword(0x6E);
            if (found)
                manager->entries[*count].tag = Script_GetValue();
            else
                manager->entries[*count].tag = found;
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
                manager->entries[*count].cachedValue = sub_0822C034(&point);
                manager->entries[*count].enabled = 1;
                (*count)++;
            }
        }
    }
}
