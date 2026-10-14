#include "scene_arc.h"
#define SceneArc_InitializeSlot sub_0803EE5C
void sub_0803EE58(void);
s32 sub_0803F730(struct SceneGroundEffect *, const struct ScenePoolVec32 *, u32);
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
    sub_0803F730(&slot->ground, &slot->position, 1);
    sub_0811B278(slot->collider, &origin, &extent, &offset, 12, 5, 5, 30, 12, sub_0803EE58, slot);
}
