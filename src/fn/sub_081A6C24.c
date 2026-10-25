#include "global.h"
struct ColliderActorVector {
    s16 x, y, z, pad;
};
struct ColliderActorExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct VanargandCameraActor {
    u8 unk_00[0x110];
    struct ColliderActorVector *targetPosition;
    u8 unk_114[0x418 - 0x114];
    struct ColliderActorVector cameraPosition;
};
void Camera_SetOffset(s32, void *);
struct ColliderActorVector *Player_GetEffectivePosition(s32);
void Vanargand_UpdateCameraOffset(struct VanargandCameraActor *actor)
{
    struct ColliderActorVector *target = actor->targetPosition;
    struct ColliderActorVector *initialPosition = &actor->cameraPosition;
    s32 x = target->x - initialPosition->x;
    s32 z = target->z - initialPosition->z;
    s32 shift = (x * x + z * z) >> 16;
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
    anchor = Player_GetEffectivePosition(0);
    out = &delta;
    position = &actor->cameraPosition;
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
