#define Hresvelgr_InitHomingShotCollider sub_081A3DA8
#include "global.h"
/* Tentative homing-shot layout, established by its collider and position uses. */
struct HomingShot {
    u8 unknown0[0x1c];
    u8 position[0x20];
    u8 collider[0x54];
};
struct ColliderHalfPair {
    u32 lo : 16;
    u32 hi : 16;
};
struct ColliderHalfTail {
    u32 lo : 16;
    u32 : 16;
};
struct ColliderVector {
    struct ColliderHalfPair xy;
    struct ColliderHalfTail z;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Hresvelgr_OnHomingShotCollision(void);
void sub_081A3DA8(struct HomingShot *shot, s32 attack, s32 force, s32 arg)
{
    struct ColliderVector size, offset;
    void *collider = shot->collider;
    size.xy.lo = 0x20;
    size.xy.hi = 0x40;
    size.z.lo = 0x20;
    offset.xy.lo = 0;
    offset.xy.hi = 0x20;
    offset.z.lo = 0;
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &size, &offset);
    Collider_SetPosition(collider, shot->position, 0);
    Collider_SetCallback(collider, Hresvelgr_OnHomingShotCollision, shot);
    Collider_SetAttackProperties(collider, attack, force, 0, 0x40000, arg);
}
