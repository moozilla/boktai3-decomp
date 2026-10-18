#define SingletonSlot7_InitContactCollider sub_081F053C
#define SingletonSlot7_BeginScaledCooldown sub_081F02EC
#include "global.h"
#include "engine.h"
struct Slot7PackedVector {
    u32 x : 16, y : 16, z : 16;
};
struct Slot7Actor {
    u8 pad0[0x1fc];
    u8 contactCollider[0x64];
    struct Slot7PackedVector position;
};
void sub_081F02EC(void);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void sub_081F053C(struct Slot7Actor *actor, s32 body)
{
    struct Slot7PackedVector size, offset;
    void *collider = actor->contactCollider;
    size.x = 256;
    size.y = 256;
    size.z = 256;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xffff0000;
    Collider_Init(collider, 0, 0x4001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &actor->position, 0);
    Collider_SetCallback(collider, sub_081F02EC, actor);
    Collider_SetBodyProperties(collider, body, 0, 0);
    Collider_Refresh(collider);
}
