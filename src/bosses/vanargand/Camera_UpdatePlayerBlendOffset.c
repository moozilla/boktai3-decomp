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
    u8 unk_00[0x54];
    struct ColliderActorVector position;
    u8 unk_5C[0xC8 - 0x5C];
    struct ColliderActorCamera camera;
};
void Camera_SetOffset(s32, void *);
struct ColliderActorVector *Player_GetEffectivePosition(s32);
static inline s32 ScaleOffset(s32 value, s32 shift)
{
    s32 result;
    if (value >= 0)
        result = value >> shift;
    else
        result = -(-value >> shift);
    return result;
}
// Tentative name: blends an actor position toward the player for the camera.
void Camera_UpdatePlayerBlendOffset(struct ColliderActor *actor)
{
    struct ColliderActorVector *player = Player_GetEffectivePosition(0);
    struct ColliderActorCamera *camera = &actor->camera;
    s32 shift = camera->shift;
    struct ColliderActorVector *anchor;
    struct ColliderActorVector delta;
    struct ColliderActorExtent offset;
    struct ColliderActorVector *out;
    struct ColliderActorVector *position;
    if (shift > 15)
        shift = 16;
    else if (shift == 0)
        shift = 1;
    out = &delta;
    anchor = &actor->position;
    position = player;
    delta.x = position->x - anchor->x;
    out->y = position->y - anchor->y;
    out->z = position->z - anchor->z;
    offset.x = actor->position.x + ScaleOffset(delta.x, shift);
    offset.z = actor->position.z + ScaleOffset(delta.z, shift);
    offset.y = player->y;
    Camera_SetOffset(8, &offset);
}
