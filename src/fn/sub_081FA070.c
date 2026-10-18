#define Ratatosk_InitBodyCollider sub_081FA070
#include "global.h"
struct ColliderVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Ratatosk {
    u8 pad0[0x1dc];
    u8 bodyCollider[0x190];
    u8 position[8];
    u8 pad374[0xb28];
    s16 attack, damage, attackKind;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Ratatosk_OnBodyCollision(void);
void sub_081FA070(struct Ratatosk *p)
{
    void *collider = p->bodyCollider;
    struct ColliderVector size, offset;
    size.x = 120;
    size.y = 180;
    size.z = 120;
    offset.x = 0;
    offset.y = 180;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, p->position, 0);
    Collider_SetCallback(collider, Ratatosk_OnBodyCollision, p);
    Collider_SetAttackProperties(collider, p->attack, p->damage, 0, 0x40000, p->attackKind);
}
