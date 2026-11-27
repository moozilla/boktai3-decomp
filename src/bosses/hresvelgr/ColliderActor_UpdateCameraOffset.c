#include "global.h"
struct ColliderActorVector {
    s16 x, y, z, pad;
};
struct ColliderActorExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct ColliderActorCamera {
    u16 unknown, shift;
};
struct ColliderActor {
    u8 pad0[0x54];
    struct ColliderActorVector position;
    u8 pad5A[0xc8 - 0x5c];
    struct ColliderActorCamera camera;
    u8 padCC[0x110 - 0xcc];
    struct ColliderActorVector *cameraAnchor;
};
void Camera_SetOffset(s32, void *);
void ColliderActor_UpdateCameraOffset(struct ColliderActor *actor)
{
    struct ColliderActorCamera *camera = &actor->camera;
    s32 shift = camera->shift;
    struct ColliderActorVector *anchor;
    struct ColliderActorVector delta;
    struct ColliderActorExtent offset;
    s32 value;
    struct ColliderActorVector *out;
    struct ColliderActorVector *position;
    if (shift > 15)
        shift = 16;
    else if (shift == 0)
        shift = 1;
    anchor = actor->cameraAnchor;
    out = &delta;
    position = &actor->position;
    delta.x = position->x - anchor->x;
    out->y = position->y - anchor->y;
    out->z = position->z - anchor->z;
    value = delta.x;
    if (value >= 0)
        value >>= shift;
    else
        value = -(-value >> shift);
    offset.x = anchor->x + value;
    value = delta.z;
    if (value >= 0)
        value >>= shift;
    else
        value = -(-value >> shift);
    offset.z = anchor->z + value;
    offset.y = anchor->y;
    Camera_SetOffset(8, &offset);
}
