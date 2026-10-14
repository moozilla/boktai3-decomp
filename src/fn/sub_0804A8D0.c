#include "tether_actor.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void sub_0821FF7C(void *, u32, u32, u32);
struct TetherChargeContactView;
void TetherActor_OnChargeContact(struct TetherChargeContactView *, void *, struct TetherActor *);
void TetherActor_InitChargeCollider(struct TetherActor *p)
{
    struct Z s1, s2;
    struct ContactCollider *q = &p->chargeCollider;
    u32 t;
    s1.a.lo = 0x5a;
    s1.a.hi = 0x20;
    s1.b.lo = 0x5a;
    s2.a.lo = 0x64;
    s2.a.hi = 0xc8;
    s2.b.lo = 0x64;
    if (p->kind == 3)
        t = 0x10;
    else if (p->kind == 9)
        t = 1;
    else if (p->kindValue == 0)
        t = 0x10;
    else
        t = 2;
    Collider_Init(q, p->entry, 0x4081, 0, t, &s1, &s2);
    sub_0821FF7C(q, 0, 0, 0);
    Collider_SetCallback(q, TetherActor_OnChargeContact, p);
    Collider_SetPosition(q, &p->position, 0);
    Collider_Refresh(q);
}
