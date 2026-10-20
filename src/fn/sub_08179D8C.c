#include "global.h"
#include "contact_actor.h"
#include "engine.h"
void ActorImpact_CopyContact(void);
#define ActorImpact_InitBodyCollider sub_08179D8C
void Collider_SetBodyProperties(void *, s32, s32, s32);
void sub_08179D8C(u8 *p, s32 resource)
{
    struct ContactPackedVector size, offset;
    void *c = p + 0x1cc;
    size.x = 128;
    size.y = 256;
    size.z = 128;
    *(u32 *)&offset = 0x800000;
    ((u32 *)&offset)[1] &= 0xffff0000;
    Collider_Init(c, 0, 0x5101, 0, 16, &size, &offset);
    Collider_SetPosition(c, p + 0x2ec, 0);
    Collider_SetCallback(c, ActorImpact_CopyContact, p);
    Collider_SetBodyProperties(c, resource, 0x802, 1);
    Collider_Refresh(c);
}
