#ifndef GUARD_PLAYER_CLONE_H
#define GUARD_PLAYER_CLONE_H
#include "scene_input.h"

struct PlayerCloneConfiguration {
    u16 flags, lifetime;
    u8 unknown4[2], angle, variant;
    u32 behaviorValue;
    void *callback;
    s16 primaryCollider[3];
    u16 unknown16;
    s16 secondaryCollider[3];
    u16 unknown1E;
};

struct PlayerCloneCollider {
    u8 unknown0[6];
    u16 flags;
};

struct PlayerCloneSlot {
    u32 timer;
    u8 unknown04[8];
    struct ContactPosition velocity, position, previousPosition, targetPosition;
    u8 unknown2C[0x54];
    struct PlayerCloneCollider collider;
    u8 unknown88[0x110];
    struct PlayerCloneConfiguration configuration;
    struct SceneInputWidget animation;
    u8 unknown218[0x60];
    struct SceneInputResource resource;
    u8 unknown298[0x6C];
    u32 flags, state;
    u8 unknown30C[0x10];
    u32 phase, unknown320, behaviorValue;
    u8 unknown328[8];
    u16 unknown330;
    u8 unknown332[0x12];
};

struct PlayerCloneManager {
    u8 task[0x18];
    u32 activeSlots;
    struct PlayerCloneSlot slots[3];
    u32 count;
};
typedef char PlayerCloneSlotSizeCheck[sizeof(struct PlayerCloneSlot) == 0x344 ? 1 : -1];
#endif
