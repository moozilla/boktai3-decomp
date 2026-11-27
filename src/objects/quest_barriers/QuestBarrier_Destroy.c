#include "objects/quest_barriers/quest_barrier.h"
void TerrainModifiers_UnregisterCell(void *);
u32 QuestBarrier_Destroy(struct BreakableProp *prop)
{
    Sprite_Remove(&prop->sprite);
    Collider_Remove(&prop->collider);
    TerrainModifiers_UnregisterCell(prop->terrainRegistration);
}
