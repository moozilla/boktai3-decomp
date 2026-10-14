#include "scene_arc.h"
u32 GroundEffect_Destroy(struct SceneGroundEffect *);
void sub_0811B394(void *);
void SceneArc_DestroySlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, s32 index)
{
    Sprite_Remove(&slot->sprite);
    GroundEffect_Destroy(&slot->ground);
    sub_0811B394(slot->collider);
}
