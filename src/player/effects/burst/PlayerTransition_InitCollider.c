#include "player/effects/orbit_effect.h"
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void PlayerTransition_InitCollider(struct PlayerOrbitEffect *p)
{
    struct ContactPosition size, offset;
    void *collider = &p->collider;
    size.x = 30000;
    size.y = 30000;
    size.z = 30000;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x3001, 0, (u16)(0x10000 << p->owner->movementRequestBlocked >> 16), &size, &offset);
    Collider_SetAttackProperties(collider, p->owner->maximumStatA, 50, 0, 0x40000, 50);
    Collider_SetCallback(collider, 0, p);
    Collider_SetPosition(collider, &p->owner->position, 0);
}
