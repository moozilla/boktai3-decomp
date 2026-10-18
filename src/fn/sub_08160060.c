#include "global.h"
#define PlayerTransition_InitCollider sub_08160060
struct TransitionVector { s32 x:16, y:16, z:16, pad:16; };
struct PlayerTransition {
    u8 unknown0[0x18];
    u8 *owner;
    u8 unknown1c[0x80];
    u8 collider[0x54];
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void sub_08160060(struct PlayerTransition *p)
{
    struct TransitionVector size, offset;
    void *collider = p->collider;
    size.x = 30000;
    size.y = 30000;
    size.z = 30000;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x3001, 0, (u16)(0x10000 << p->owner[0x2c] >> 16), &size, &offset);
    Collider_SetAttackProperties(collider, *(u16 *)(p->owner + 0x426), 50, 0, 0x40000, 50);
    Collider_SetCallback(collider, 0, p);
    Collider_SetPosition(collider, p->owner + 0x30, 0);
}
