#include "global.h"
#define CollisionVolume_InitCollider sub_08163C00
struct CollisionVolumeVector {
    u32 x : 16, y : 16, z : 16;
};
struct CollisionVolume {
    u8 task[0x18];
    u8 collider[0x58];
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
void sub_08163C00(struct CollisionVolume *p)
{
    struct CollisionVolumeVector size, offset;
    void *collider = p->collider;
    size.x = 30000;
    size.y = 30000;
    size.z = 30000;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x3001, 0, 1, &size, &offset);
    Collider_SetAttackProperties(collider, 100, 50, 0, 0x40000, 50);
    Collider_SetCallback(collider, 0, p);
}
