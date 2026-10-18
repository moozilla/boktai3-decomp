#include "global.h"
struct Ooze {
    u8 pad0[0x68];
    u8 collider[0xb0];
    s16 attack, element, power;
    u8 pad11E[4];
    s16 variant;
    u8 pad124[0x20];
    s16 orientation;
};
struct OozeColliderVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Ooze_OnCollision(void);
void Ooze_InitAttackCollider(struct Ooze *p)
{
    struct OozeColliderVector size, offset;
    void *collider = p->collider;
    if (p->variant == 0) {
        size.x = 128;
        size.y = 128;
        size.z = 128;
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
    } else if (p->orientation == 0) {
        size.x = 160;
        size.y = 128;
        size.z = 128;
        offset.x = 128;
        offset.y = 0;
        offset.z = 0;
    } else {
        size.x = 128;
        size.y = 128;
        size.z = 160;
        offset.x = 0;
        offset.y = 0;
        offset.z = 128;
    }
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(collider, Ooze_OnCollision, p);
    Collider_SetAttackProperties(collider, p->attack, p->element, 0, 0, p->power);
}
