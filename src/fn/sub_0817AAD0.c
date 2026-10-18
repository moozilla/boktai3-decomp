#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
#define Collider_SetExtents Collider_SetAttackProperties
struct SceneAttackCollider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x3A];
    u8 attack;
};
struct SceneVolume {
    u8 pad0[0x110];
    void *position;
    u8 pad114[0x64];
    struct SceneAttackCollider collider;
};
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
s32 Collider_Refresh(void *);
void SceneVolume_ConfigureAttackCollider(struct SceneVolume *volume, s32 radius, s32 attack,
                                         s32 height)
{
    struct SceneAttackCollider *collider = &volume->collider;
    u32 mask = 0x100;
    collider->flags |= mask;
    Collider_SetPosition(collider, volume->position, 0);
    Collider_SetAttackProperties(collider, 0, radius, 0, 0x40, height);
    volume->collider.attack = attack;
    Collider_Refresh(collider);
}
