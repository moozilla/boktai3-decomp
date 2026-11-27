#include "bosses/hresvelgr/hresvelgr_shot.h"
#include "scene/collision/packed_contact_vector.h"

void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
s32 Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Hresvelgr_OnShotCollision(void);
s32 Hresvelgr_InitShotCollider(struct HresvelgrShot *shot, s32 a, s32 b, s32 c)
{
    struct PackedContactVector size;
    struct PackedContactVector offset;
    struct ContactCollider *collider = &shot->collider;
    size.x = 8;
    size.y = 16;
    size.z = 8;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &shot->sprite.position, 0);
    Collider_SetCallback(collider, Hresvelgr_OnShotCollision, shot);
    return Collider_SetAttackProperties(collider, a, b, 0, 0, c);
}
