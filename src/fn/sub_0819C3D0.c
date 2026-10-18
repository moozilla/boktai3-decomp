#include "global.h"
#include "engine.h"
#define ColliderActor_InitBodyCollider sub_0819C3D0
struct ColliderActorExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct ColliderActorRotation {
    s16 x, y, z;
};
struct ColliderActor {
    u8 pad0[0x54];
    s16 position[3];
    u8 pad5A[0x248 - 0x5a];
    u8 collider[0x38c - 0x248];
    struct ColliderActorExtent offset;
    struct ColliderActorRotation rotation;
};
void sub_0819C0D0(void);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void sub_0819C3D0(struct ColliderActor *actor, s32 mode)
{
    void *collider = actor->collider;
    struct ColliderActorExtent size, offset;
    size.x = 256;
    size.y = 256;
    size.z = 256;
    offset.x = 0;
    offset.y = 320;
    offset.z = 0;
    actor->offset = offset;
    {
        struct ColliderActorRotation *rotation = &actor->rotation;
        s16 zero = 0, angle = 64;
        rotation->x = zero;
        rotation->y = angle;
        rotation->z = zero;
    }
    Collider_Init(collider, 0, 0x5101, 0, 16, &size, &offset);
    Collider_SetPosition(collider, actor->position, 0);
    Collider_SetCallback(collider, sub_0819C0D0, actor);
    Collider_SetBodyProperties(collider, mode, 0x810, 32);
    Collider_Refresh(collider);
}
