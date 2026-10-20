#include "global.h"

#define Explosion_InitCollider sub_081DB030

struct ExplosionSize {
    u32 x : 16, y : 16, z : 16;
};
void Collider_Init(u8 *, u32, u32, u32, u32, void *, void *);
void Collider_SetAttackProperties(u8 *, s32, s32, s32, s32, s32);
void Collider_SetCallback(u8 *, u32, u8 *);

void Explosion_InitCollider(u8 *obj, u8 *collider, u32 flag, s32 a3, s32 a4, s32 a5, s32 a6)
{
    struct ExplosionSize size, offset;

    size.x = 120;
    size.y = 120;
    size.z = 120;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x2001, 0, (u16)flag, &size, &offset);
    Collider_SetAttackProperties(collider, a3, a4, 0x210, a5, a6);
    Collider_SetCallback(collider, 0, obj);
    if (a3 == 0 && a4 == 0)
        *(u16 *)(obj + 0xb8) = 1;
}
