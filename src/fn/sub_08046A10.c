#include "orbit_contact_actor.h"
#define OrbitContactActor_Initialize sub_08046A10
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
s32 OrbitContactActor_Initialize(struct OrbitContactActor *actor,
                                 const struct ContactPosition *center, u32 resourceValue)
{
    struct ContactSwarmGraphics *graphics;
    struct ContactSwarmSprite *sprite;
    struct ContactPackedVector bounds, offset;
    struct ContactCollider *collider;
    s32 result;
    actor->center = *center;
    graphics = &actor->graphics;
    if (AnimData_Load(graphics, 0x4148)) {
        sprite = &actor->sprite;
        Sprite_Init(sprite, graphics, 1);
        sprite->priority = 2;
        AnimData_SelectTiles(graphics, 0x15F);
        bounds.x = 128;
        bounds.y = 128;
        bounds.z = 128;
        offset.x = 0;
        offset.y = 128;
        offset.z = 0;
        collider = &actor->collider;
        Collider_Init(collider, 0, 0x2001, 0, 16, &bounds, &offset);
        Collider_SetCallback(collider, NULL, actor);
        sub_0821FF58(collider, resourceValue, 50, 0, 0x60000, 30);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}
