#define Ratatosk_InitWideBodyCollider sub_081FA388
#include "global.h"
struct ColliderVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Ratatosk {
    u8 pad0[0x2d8];
    u8 wideBodyCollider[0x94];
    u8 position[8];
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetBodyProperties(void *, s32, s32, s32);
s32 Collider_Refresh(void *);
void sub_081F9E20(void);
void sub_081FA388(struct Ratatosk *p)
{
    struct ColliderVector size, offset;
    void *collider = p->wideBodyCollider;
    size.x = 140;
    size.y = 180;
    size.z = 140;
    offset.x = 0;
    offset.y = 180;
    offset.z = 0;
    Collider_Init(collider, 0, 0x4005, 0, 16, &size, &offset);
    Collider_SetPosition(collider, p->position, 0);
    Collider_SetCallback(collider, sub_081F9E20, p);
    Collider_SetBodyProperties(collider, 0, 0x804, 8);
    Collider_Refresh(collider);
}
