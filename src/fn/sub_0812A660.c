#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Packed {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct Vec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};
union Vector {
    struct Packed bits;
    struct Vec halves;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Collider_SetBodyProperties(void *, u16, u32, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void ActorEffect_OrientFromPositions(void);
void LinkedEffect_ApplyVariant(void);
void ActorEffect_InitColliders(u8 *arg, u32 arg1, u32 arg2, u32 arg3, u32 extra)
{
    u8 **actorReference;
    u8 *p = arg;
    u32 kind = arg1;
    register u32 power asm("r9") = arg2;
    register u32 value asm("r8") = arg3;
    union Vector size;
    u32 colliderValue;
    union Vector offset;
    register u8 *collider asm("r4");
    u8 *position;
    size.bits.x = 64;
    size.bits.y = 128;
    actorReference = &p;
    colliderValue = 16;
    size.bits.z = 64;
    offset.bits.x = 0;
    offset.bits.y = 0;
    offset.bits.z = 0;
    collider = p + 0x58;
    Collider_Init(collider, 0, 0x2001, 0, colliderValue, &size, &offset);
    Collider_SetAttackProperties(collider, kind, power, 0, value, extra);
    Collider_SetCallback(collider, ActorEffect_OrientFromPositions, p);
    position = (*actorReference) + 0x1c;
    Collider_SetPosition(collider, position, 0);
    size.halves.x = 92;
    size.halves.y = 128;
    size.halves.z = 92;
    collider += 0x54;
    Collider_Init(collider, 0, 0x4005, 0, colliderValue, &size, &offset);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    colliderValue = 0;
    Collider_SetCallback(collider, LinkedEffect_ApplyVariant, p);
    Collider_SetPosition(collider, position, colliderValue);
    *((u16 *)(p + 0xf0)) = 60;
}
