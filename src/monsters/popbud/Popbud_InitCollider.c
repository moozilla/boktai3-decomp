#include "global.h"
#include "scene/collision/packed_contact_halfwords.h"
#include "scene/collision/packed_collider_extents.h"
void Collider_Init(u8 *, u16, u32, u32, u32, struct PackedColliderExtents *,
                   struct PackedColliderExtents *);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
s32 Collider_Refresh(u8 *);
void Combat_ApplyHitReaction(void);
void Collider_SetBodyProperties(void *, u32, u32, u32);
struct Q {
    u8 pad[6];
    u16 flags;
};
void Popbud_InitCollider(u8 *p)
{
    struct PackedColliderExtents a, b;
    u8 *q = p + 0x6c;
    a.xy.lo = 100;
    a.xy.hi = 120;
    a.zAndPad.lo = 100;
    *(u32 *)&b.xy = 0x780000;
    *(u32 *)&b.zAndPad &= 0xFFFF0000;
    Collider_Init(q, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(q, p + 0x40, 0);
    Collider_SetCallback(q, Combat_ApplyHitReaction, p);
    Collider_SetBodyProperties(q, 0, 0, 0);
    Collider_Refresh(q);
    ((struct Q *)q)->flags |= 4;
}
