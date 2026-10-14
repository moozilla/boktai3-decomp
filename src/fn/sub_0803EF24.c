#include "scene_arc.h"
void GroundEffect_DisableProjection(struct SceneGroundEffect *);
void SceneArc_ReleaseSlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, u32 index)
{
    u32 mask;
    slot->sprite.flags |= 1;
    GroundEffect_DisableProjection(&slot->ground);
    mask = 1 << index;
    manager->activeMask &= ~mask;
}
