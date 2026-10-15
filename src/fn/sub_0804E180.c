#include "area_effect.h"
// MATCHING_CONCESSION: register-pinning
void AreaEffect_SetBounds(struct AreaEffectEntry *entry, s32 kind,
                          struct AreaEffectPosition *position, s32 rotated)
{
    s32 xRadius, zRadius;
    register u32 x asm("r2");
    register u32 z asm("r1");
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
    x = xRadius + x;
    entry->xMax = x;
    z = zRadius + z;
    entry->zMax = z;
}
