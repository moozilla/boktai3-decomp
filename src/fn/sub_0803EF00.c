#include "scene_arc.h"
u32 GroundEffect_Destroy(struct SceneGroundEffect *);
void SpatialIndex_RemoveEntry(void *);
void SceneArc_DestroySlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, s32 index)
{
    Sprite_Remove(&slot->sprite);
    GroundEffect_Destroy(&slot->ground);
    SpatialIndex_RemoveEntry(slot->collider);
}
