#ifndef GUARD_HRESVELGR_SHOT_H
#define GUARD_HRESVELGR_SHOT_H

#include "engine/video/sprite.h"
#include "scene/collision/contact_collider.h"

struct HresvelgrShot {
    struct DisplaySequenceSprite sprite;
    struct ContactCollider collider;
    s16 velocityX, velocityY, velocityZ;
    u8 unknown86[4];
    u16 lifetime;
    u8 unknown8C[4];
};

typedef char HresvelgrShotSizeCheck[sizeof(struct HresvelgrShot) == 0x90 ? 1 : -1];

s32 Hresvelgr_InitShotCollider(struct HresvelgrShot *, s32, s32, s32);

#endif
