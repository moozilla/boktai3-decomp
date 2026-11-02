#include "homing_projectile.h"

struct ColliderHalfwords {
    s32 lo : 16;
    s32 hi : 16;
};
struct ColliderVector {
    struct ColliderHalfwords a, b;
};
struct ProjectileCollider {
    u8 unknown[6];
    u16 flags;
};
static inline void ClearPosition(void *position)
{
    u32 zero = 0;
    CpuSet(&zero, position, 0x05000002);
}
void Sprite_InitUnlinked(void *, void *, s32);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetBodyProperties(void *, s32, s32, s32);
s32 Collider_Refresh(void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void HomingProjectile_BeginImpact(void *, void *, struct HomingSlot *);
void HomingProjectile_OnSecondaryCollision(void);

void HomingProjectile_InitializeSlot(void *manager, struct HomingSlot *p, u32 slotIndex)
{
    struct HomingParams *params = &p->params;
    struct ColliderVector size, offset;
    u8 *collider;
    u16 *sizeComponents, *offsetComponents;
    HomingProjectile_InitParams(params);
    p->lifetime = 0;
    ClearPosition(&p->velocity);
    Sprite_InitUnlinked(&p->sprite, &p->graphics, 1);
    p->slotIndex = slotIndex;
    collider = (u8 *)&p->primaryCollider;
    size.a.lo = 40;
    size.a.hi = 128;
    size.b.lo = 40;
    offset.a.lo = 0;
    offset.a.hi = 128;
    offset.b.lo = 0;
    Collider_Init(collider, 0, 0x4001, 0, 16, &size, &offset);
    Collider_SetCallback(collider, HomingProjectile_BeginImpact, p);
    Collider_SetBodyProperties(collider, params->body, 0x402, 1);
    Collider_Refresh(collider);
    ((struct ProjectileCollider *)collider)->flags |= 4;
    collider -= 0x54;
    sizeComponents = (u16 *)&size;
    offsetComponents = (u16 *)&offset;
    sizeComponents[0] = 64;
    sizeComponents[1] = 128;
    sizeComponents[2] = 64;
    offsetComponents[0] = 0;
    offsetComponents[1] = 128;
    offsetComponents[2] = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(collider, HomingProjectile_OnSecondaryCollision, p);
    Collider_SetAttackProperties(collider, params->attack, params->element, 0, 0,
                                 params->attackFlags);
}
