#include "area_effect.h"
void AreaEffect_SetBounds(struct AreaEffectEntry *entry, s32 kind,
                          struct AreaEffectPosition *position, s32 rotated)
{
    s32 xRadius, zRadius;
    u32 x;
    u32 z;
    if (rotated) {
        xRadius = gUnk_08605A78[kind];
        zRadius = gUnk_08605A80[kind];
    } else {
        xRadius = gUnk_08605A80[kind];
        zRadius = gUnk_08605A78[kind];
    }
    x = (u16)position->x;
    entry->xMin = x - xRadius;
    z = (u16)position->z;
    entry->zMin = z - zRadius;
    entry->xMax = xRadius + x;
    entry->zMax = zRadius + z;
}
