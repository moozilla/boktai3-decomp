#define Hresvelgr_TickShot sub_081A33B8
#include "global.h"
struct HresvelgrShot {
    u32 flags;
    u8 unknown04[0x18];
    u16 x;
    u16 unknown1E;
    u16 z;
    u8 unknown22[0xA];
    u8 collider[0x54];
    u16 velocityX;
    u16 unknown82;
    u16 velocityZ;
    u8 unknown86[4];
    u16 lifetime;
};
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void Hresvelgr_ReleaseShot(struct HresvelgrShot *);
void sub_081A33B8(void *pool, struct HresvelgrShot *shot, u32 phase)
{
    if (phase == 0) {
        void *collider = shot->collider;
        Collider_SetPosition(collider, &shot->x, 0);
        Collider_Refresh(collider);
    } else {
        shot->x += shot->velocityX;
        shot->z += shot->velocityZ;
    }
    if (--shot->lifetime == 0) {
        Hresvelgr_ReleaseShot(shot);
    } else if (shot->lifetime == 16) {
        if (phase & 1)
            shot->flags |= 0x200;
        else
            shot->flags |= 0x400;
    }
}
