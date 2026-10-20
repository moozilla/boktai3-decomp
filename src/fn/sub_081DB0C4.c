#include "global.h"

#define Explosion_InitColliderWithFlags sub_081DB0C4

struct ExplosionSize {
    u32 x : 16, y : 16, z : 16;
};
void Collider_Init(u8 *, u32, u32, u32, u32, void *, void *);
void Collider_SetAttackProperties(u8 *, s32, s32, s32, s32, s32);
void Collider_SetCallback(u8 *, u32, u8 *);

void Explosion_InitColliderWithFlags(u8 *obj, u8 *collider, u32 flag, s32 a3, s32 a4, s32 a5,
                                     s32 a6, s32 a7)
{
    struct ExplosionSize size, offset;

    size.x = 120;
    size.y = 120;
    size.z = 120;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x2001, 0, (u16)flag, &size, &offset);
    Collider_SetAttackProperties(collider, a4, a5, 0x200 | a3, a6, a7);
    Collider_SetCallback(collider, 0, obj);
    if (a4 == 0 && a5 == 0)
        *(u16 *)(obj + 0xb8) = 1;
}
