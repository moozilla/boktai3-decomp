#include "scene_arc.h"
void SceneArc_HandleCollision(void);
s32 GroundEffect_Initialize(struct SceneGroundEffect *, const struct ScenePoolVec32 *, u32);
void sub_0811B278(void *, void *, void *, void *, u32, u32, u32, u32, u32, void (*)(void), void *);
struct ArcCollisionVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
void SceneArc_InitializeSlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, s32 index)
{
    struct ArcCollisionVector origin, extent, offset;
    slot->frame = 0;
    AnimData_Load(slot->animationData, 0xe74b);
    Sprite_Init(&slot->sprite, slot->animationData, 0x11);
    slot->start.x = 0;
    slot->start.y = 0;
    slot->start.z = 0;
    origin.x = 0;
    origin.y = 0;
    origin.z = 0;
    extent.x = 8;
    extent.y = 4;
    extent.z = 8;
    offset.x = 0;
    offset.y = 4;
    offset.z = 0;
    GroundEffect_Initialize(&slot->ground, &slot->position, 1);
    sub_0811B278(slot->collider, &origin, &extent, &offset, 12, 5, 5, 30, 12,
                 SceneArc_HandleCollision, slot);
}
