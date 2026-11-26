#include "scene/collision/contact_actor.h"
#include "monsters/cannons/wind_cannon.h"
void WindCannon_ApplyHitReaction(void);
void Collider_SetBodyProperties(void *, u32, u32, u32);
// confidence=low: VulcanCannon_InitCollider's setup for the fan cannon; the signed flag at 0x11A
// swaps which extent is 128 wide.
void sub_082093F8(struct FanCannon *p)
{
    struct ContactPackedVector a, b;
    struct ContactCollider *q = &p->collider;
    if (p->direction == 0) {
        a.x = 0x80;
        a.y = 120;
        a.z = 100;
    } else {
        a.x = 100;
        a.y = 120;
        a.z = 0x80;
    }
    b.x = 0;
    b.y = 0x80;
    b.z = 0;
    Collider_Init(q, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(q, &p->sprite.position, 0);
    Collider_SetCallback(q, WindCannon_ApplyHitReaction, p);
    Collider_SetBodyProperties(q, 0, (u16)p->f12e, (u16)p->hudB);
    Collider_Refresh(q);
    q->flags |= 4;
}
