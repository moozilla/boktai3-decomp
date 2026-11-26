#include "global.h"
struct HresvelgrShot {
    u8 unknown00[0x1C];
    u16 position[3];
    u8 unknown22[0xA];
    u8 collider[0x54];
};
struct HresvelgrColliderVector {
    u32 x : 16, y : 16, z : 16;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
s32 Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Hresvelgr_OnShotCollision(void);
s32 Hresvelgr_InitShotCollider(struct HresvelgrShot *shot, s32 a, s32 b, s32 c)
{
    struct HresvelgrColliderVector size;
    struct HresvelgrColliderVector offset;
    void *collider = shot->collider;
    size.x = 8;
    size.y = 16;
    size.z = 8;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, shot->position, 0);
    Collider_SetCallback(collider, Hresvelgr_OnShotCollision, shot);
    return Collider_SetAttackProperties(collider, a, b, 0, 0, c);
}
