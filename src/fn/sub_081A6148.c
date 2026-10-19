#include "global.h"
#include "engine.h"
struct PositionedEffect {
    u8 pad0[0x1ac];
    u8 position[8];
    u8 pad1[0x510];
    u8 collider[0x42];
    u8 contactFlags;
};
struct ContactVector {
    u32 x : 16, y : 16, z : 16;
};
void sub_081A6108(void);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void PositionedEffect_InitContactCollider(struct PositionedEffect *actor)
{
    struct ContactVector size, offset;
    void *collider = actor->collider;
    size.x = 256;
    size.y = 256;
    size.z = 256;
    *(u32 *)&offset = 0x800000;
    ((u32 *)&offset)[1] &= 0xffff0000;
    Collider_Init(collider, 0, 0x5001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, actor->position, 0);
    Collider_SetCallback(collider, sub_081A6108, actor);
    Collider_SetBodyProperties(collider, 10, 2048, 0);
    {
        u32 flags = 32;
        actor->contactFlags = flags;
    }
    Collider_Refresh(collider);
}
