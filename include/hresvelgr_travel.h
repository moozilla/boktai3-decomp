#ifndef GUARD_HRESVELGR_TRAVEL_H
#define GUARD_HRESVELGR_TRAVEL_H

#include "scene_vec.h"

struct HresvelgrTravelActor {
    u8 pad0[0x51];
    u8 facing;
    u8 pad52[2];
    struct SceneVec position;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[8];
    u8 facing4, facing8;
    u8 padAA[5];
    u8 finished;
    u8 padB0;
    u8 changed, pulse;
    u8 padB3[0x51];
    void *callback;
    u8 pad108[8];
    struct SceneVec *origin;
    u8 pad114[0x60];
    u8 animationData[0x20], animation[0x20];
    u8 pad1B4[0x13C];
    u8 action[0x10];
    u8 pad300[0x2B];
    u8 side;
    u8 pad32C[0x5E];
    s16 travel;
    u8 pad38C[0x10];
    struct SceneVec target;
};

s32 Hresvelgr_RefreshTarget(struct HresvelgrTravelActor *);

#endif
