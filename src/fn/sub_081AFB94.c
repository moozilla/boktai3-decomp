#define ActorStatus_InitializeCollider sub_081AFB94
#include "global.h"
#include "engine.h"
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void sub_081AFABC(void);
struct ActorStatusExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct ActorStatus {
    u8 pad0[0x1C];
    u8 position[8];
    u8 pad24[0x90];
    u8 collider[0xA8];
    u32 mode;
};
void sub_081AFB94(struct ActorStatus *status, s32 power, s32 force, s32 value)
{
    void *collider = status->collider;
    struct ActorStatusExtent size, offset;
    size.x = 0x70;
    size.y = 0x100;
    size.z = 0x70;
    offset.x = 0;
    offset.y = 0x80;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &size, &offset);
    Collider_SetPosition(collider, status->position, 0);
    Collider_SetCallback(collider, sub_081AFABC, status);
    if (status->mode == 0)
        Collider_SetAttackProperties(collider, power, force, 0, 1, value);
    else
        Collider_SetAttackProperties(collider, power, force, 0, 2, value);
}
