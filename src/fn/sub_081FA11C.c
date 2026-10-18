#include "global.h"
struct ColliderVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Ratatosk {
    u8 pad0[0x188];
    u8 hurtCollider[0x1e4];
    u8 position[8];
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetBodyProperties(void *, s32, s32, s32);
s32 Collider_Refresh(void *);
void sub_081F9BEC(void);
void Ratatosk_InitHurtCollider(struct Ratatosk *p, s32 resource)
{
    struct ColliderVector size, offset;
    void *collider = p->hurtCollider;
    size.x = 96;
    size.y = 180;
    size.z = 96;
    offset.x = 0;
    offset.y = 180;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5101, 0, 16, &size, &offset);
    Collider_SetPosition(collider, p->position, 0);
    Collider_SetCallback(collider, sub_081F9BEC, p);
    Collider_SetBodyProperties(collider, resource, 0x804, 8);
    Collider_Refresh(collider);
}
