#include "global.h"
#include "engine/engine.h"
struct VanargandHeadPackedVector {
    u32 x : 16, y : 16, z : 16;
};
struct VanargandHeadActorView {
    u8 pad0[0x1fc];
    u8 contactCollider[0x64];
    struct VanargandHeadPackedVector position;
};
void VanargandHead_BeginScaledCooldown(void);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void VanargandHead_InitContactCollider(struct VanargandHeadActorView *actor, s32 body)
{
    struct VanargandHeadPackedVector size, offset;
    void *collider = actor->contactCollider;
    size.x = 256;
    size.y = 256;
    size.z = 256;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xffff0000;
    Collider_Init(collider, 0, 0x4001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &actor->position, 0);
    Collider_SetCallback(collider, VanargandHead_BeginScaledCooldown, actor);
    Collider_SetBodyProperties(collider, body, 0, 0);
    Collider_Refresh(collider);
}
