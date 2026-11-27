#include "bosses/hresvelgr/hresvelgr_shot.h"

void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void Hresvelgr_ReleaseShot(struct HresvelgrShot *);
void Hresvelgr_TickShot(void *pool, struct HresvelgrShot *shot, u32 phase)
{
    if (phase == 0) {
        struct ContactCollider *collider = &shot->collider;
        Collider_SetPosition(collider, &shot->sprite.position, 0);
        Collider_Refresh(collider);
    } else {
        shot->sprite.position.x += shot->velocityX;
        shot->sprite.position.z += shot->velocityZ;
    }
    if (--shot->lifetime == 0) {
        Hresvelgr_ReleaseShot(shot);
    } else if (shot->lifetime == 16) {
        if (phase & 1)
            shot->sprite.flags |= 0x200;
        else
            shot->sprite.flags |= 0x400;
    }
}
