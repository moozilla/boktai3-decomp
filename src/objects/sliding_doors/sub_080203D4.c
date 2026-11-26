#include "objects/sliding_doors/scene_object_controller.h"
void TerrainModifiers_UnregisterCell(struct TerrainCellOverride *);
void ResourceRegistry_UnregisterResource(int);
int SlidingDoors_Destroy(struct SceneObjPool *p)
{
    int i;
    for (i = 0; i < p->count; i++) {
        struct SceneObjSlot *slot = &p->slots[i];
        if (slot->terrain) {
            TerrainModifiers_UnregisterCell(slot->terrain);
            Heap_Free(slot->terrain);
        }
        Sprite_Remove(&slot->sprite);
    }
    if (p->separatePalettes) {
        Heap_Free(p->paletteAnimationData[0]);
        Heap_Free(p->paletteAnimationData[1]);
        Heap_Free(p->paletteAnimationData[2]);
        Heap_Free(p->paletteAnimationData[3]);
        Heap_Free(p->paletteAnimationData[4]);
        Heap_Free(p->paletteAnimationData[5]);
        Heap_Free(p->paletteAnimationData[6]);
    }
    Heap_Free(p->slots);
    ResourceRegistry_UnregisterResource(0x85e6);
    return 0;
}
