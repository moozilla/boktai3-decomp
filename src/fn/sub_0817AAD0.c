#include "scene_volume.h"

void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
s32 Collider_Refresh(void *);

void SceneVolume_ConfigureAttackCollider(struct SceneVolumeActor *volume, s32 radius, s32 attack,
                                         s32 height)
{
    struct ContactCollider *collider = &volume->collider;
    u32 mask = 0x100;
    collider->flags |= mask;
    Collider_SetPosition(collider, volume->positionSource, 0);
    Collider_SetAttackProperties(collider, 0, radius, 0, 0x40, height);
    volume->collider.direction = attack;
    Collider_Refresh(collider);
}
