#include "global.h"
#include "contact_actor.h"
#include "engine.h"
void sub_08179CFC(void);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void ActorImpact_InitAttackCollider(u8 *p)
{
    struct ContactPackedVector size, offset;
    void *c = p + 0x178;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    *(u32 *)&offset = 0x800000;
    ((u32 *)&offset)[1] &= 0xffff0000;
    Collider_Init(c, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(c, p + 0x2ec, 0);
    Collider_SetCallback(c, sub_08179CFC, p);
    Collider_SetAttackProperties(c, 0, 0, 0, 0, 0);
}
