#include "breakable_prop.h"
void TerrainModifiers_UnregisterCell(void *);
u32 BreakableProp_Destroy(struct BreakableProp *prop)
{
    Sprite_Remove(&prop->sprite);
    Collider_Remove(&prop->collider);
    TerrainModifiers_UnregisterCell(prop->terrainRegistration);
}
