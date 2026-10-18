#define ColliderSprite_Unlink TimedHazard_UnlinkInstance
#define gColliderSpritePool   gUnk_02000474
#include "global.h"
struct ColliderSprite {
    u8 unknown0[0x18];
    void *collider;
    void *resource;
    u8 unknown20[0x1c];
    u8 sprite[1];
};
extern void *gUnk_02000474;
void Sprite_Remove(void *);
void Collider_Remove(void *);
s32 Heap_Free(void *);
u32 TimedHazard_UnlinkInstance(void *, struct ColliderSprite *);
s32 ColliderSprite_Destroy(struct ColliderSprite *object)
{
    Sprite_Remove(object->sprite);
    if (gUnk_02000474)
        TimedHazard_UnlinkInstance(gUnk_02000474, object);
    if (object->collider) {
        Collider_Remove(object->collider);
        Heap_Free(object->collider);
    }
    if (object->resource)
        Heap_Free(object->resource);
    return Heap_Free(object);
}
