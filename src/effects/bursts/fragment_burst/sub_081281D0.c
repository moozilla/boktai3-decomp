#include "global.h"
#include "scene/collision/packed_contact_halfwords.h"
#include "scene/collision/packed_contact_vector.h"
void Collider_Init(u8 *, u32, u32, u32, u32, struct PackedContactVector *,
                   struct PackedContactVector *);
void Collider_SetAttackProperties(u8 *, u32, u32, u32, u32, u32);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void FragmentBurst_OnContact(void);
// confidence=low: sets up the fragment burst's attack collider at +0x100.
void sub_081281D0(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    struct PackedContactVector s1, s2;
    u8 *q;
    s1.x = 0x80;
    s1.y = 0x20;
    s1.z = 0x80;
    s2.x = 0;
    s2.y = 0;
    s2.z = 0;
    q = p + 0x100;
    Collider_Init(q, 0, 0x2001, 0, 0x10, &s1, &s2);
    Collider_SetAttackProperties(q, a, b, 0, c, d);
    Collider_SetCallback(q, FragmentBurst_OnContact, p);
}
