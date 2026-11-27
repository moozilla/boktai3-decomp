#include "objects/quest_barriers/quest_barrier.h"
#include "scene/collision/packed_contact_halfwords.h"
#include "scene/collision/packed_collider_extents.h"
void QuestBarrier_InitializeCollider(struct BreakableProp *prop, u32 x, void *y)
{
    struct PackedColliderExtents s1, s2;
    u8 *q = (u8 *)&prop->collider;
    s1.xy.lo = 0x80;
    s1.xy.hi = 0x80;
    s1.zAndPad.lo = 0x80;
    *(u32 *)&s2.xy = 0x800000;
    *(u32 *)&s2.zAndPad &= 0xFFFF0000;
    Collider_Init(q, x, 0x4001, 0, 0x20, &s1, &s2);
    Collider_SetCallback(q, QuestBarrier_RequestBreakOnContact, prop);
    Collider_SetPosition(q, y, 0);
    Collider_Refresh(q);
}
