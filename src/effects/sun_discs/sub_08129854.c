#include "global.h"
#include "scene/collision/packed_contact_halfwords.h"
#include "scene/collision/packed_contact_vector.h"
void Collider_Init(u8 *, u32, u32, u32, u32, struct PackedContactVector *,
                   struct PackedContactVector *);
void Collider_SetAttackProperties(u8 *, u32, u32, u32, u32, u32);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
extern u8 SpriteActor_AdvanceContactState[]; // Thumb address of a `bx lr` stub at 0x0812CE90
void sub_08129854(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    struct PackedContactVector s1, s2;
    u8 *q;
    s1.x = 0x20;
    s1.y = 0x20;
    s1.z = 0x20;
    s2.x = 0;
    s2.y = 0x20;
    s2.z = 0;
    q = p + 0x9C;
    Collider_Init(q, 0, 0x2001, 0, 0x10, &s1, &s2);
    Collider_SetAttackProperties(q, a, b, 0, c, d);
    Collider_SetCallback(q, (void (*)(void))SpriteActor_AdvanceContactState, p);
    Collider_SetPosition(q, p + 0x1c, 0);
}
