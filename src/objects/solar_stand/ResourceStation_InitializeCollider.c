#include "global.h"
#include "scene/collision/packed_contact_halfwords.h"
#include "scene/collision/packed_collider_extents.h"
void Collider_Init(u8 *, u32, u32, u32, u32, struct PackedColliderExtents *,
                   struct PackedColliderExtents *);
void Collider_SetBodyProperties(u8 *, u32, u32, u32);
void Collider_SetCallback(u8 *, u32, u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
s32 Collider_Refresh(u8 *);
s32 ResourceStation_InitializeCollider(u8 *p)
{
    struct PackedColliderExtents s1, s2;
    u8 *q = p + 0x200;
    s1.xy.lo = 0x64;
    s1.xy.hi = 0x96;
    s1.zAndPad.lo = 0x64;
    *(u32 *)&s2.xy = 0x960000;
    *(u32 *)&s2.zAndPad &= 0xFFFF0000;
    Collider_Init(q, *(u16 *)(p + 0x260), 0x4001, 0, 0, &s1, &s2);
    Collider_SetBodyProperties(q, 0, 0, 0);
    Collider_SetCallback(q, 0, p);
    Collider_SetPosition(q, p + 0x34, 0);
    return Collider_Refresh(q);
}
