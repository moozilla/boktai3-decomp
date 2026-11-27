#include "global.h"
struct ColliderVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Ratatosk {
    u8 pad0[0x284];
    u8 outerCollider[0xe8];
    u8 position[8];
    u8 pad374[0xb28];
    s16 attack, damage, attackKind;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Ratatosk_OnOuterCollision(void);
void Ratatosk_InitOuterCollider(struct Ratatosk *p)
{
    void *collider = p->outerCollider;
    struct ColliderVector size, offset;
    size.x = 130;
    size.y = 180;
    size.z = 130;
    offset.x = 0;
    offset.y = 180;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, p->position, 0);
    Collider_SetCallback(collider, Ratatosk_OnOuterCollision, p);
    Collider_SetAttackProperties(collider, p->attack, p->damage, 0, 0x40000, p->attackKind);
}
