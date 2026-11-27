#include "objects/quest_barriers/quest_barrier.h"
#include "scene/collision/packed_contact_halfwords.h"
struct PropVector {
    struct PackedContactHalfwords a, b;
};
void QuestBarrier_InitializeCollider(struct BreakableProp *prop, u32 x, void *y)
{
    struct PropVector s1, s2;
    u8 *q = (u8 *)&prop->collider;
    s1.a.lo = 0x80;
    s1.a.hi = 0x80;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    Collider_Init(q, x, 0x4001, 0, 0x20, &s1, &s2);
    Collider_SetCallback(q, QuestBarrier_RequestBreakOnContact, prop);
    Collider_SetPosition(q, y, 0);
    Collider_Refresh(q);
}
