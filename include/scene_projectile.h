#ifndef GUARD_SCENE_PROJECTILE_H
#define GUARD_SCENE_PROJECTILE_H

#include "global.h"

/* One entry in the ten-slot scene projectile pool. */
struct SceneProjectile {
    u32 age;
    u32 flags;
    u8 unknown08[0x20];
    u16 stateUnknown;
    u16 lifetime;
    u8 unknown2C[3];
    s8 pulse;
    s32 phase;
    u8 unknown34[0x42];
    s16 contactMode;
    s16 contactTimer;
    u8 unknown7A[0x22];
    u32 spriteFlags;
    u8 unknownA0[2];
    u8 spriteRotation;
    u8 unknownA3[0x15];
    s32 position[3];
    u8 unknownC4[0xB1];
    u8 direction;
    u8 unknown176[2];
    s16 contactCount;
};

/* Contact-state view beginning at SceneProjectile.stateUnknown. */
struct SceneProjectileContactState {
    u16 unknown;
    u16 lifetime;
    u8 unknown04[0x4A];
    s16 mode;
    s16 timer;
};

struct SceneProjectileContact {
    u8 unknown00[10];
    u16 kind;
};

struct SceneProjectilePool {
    u8 task[0x18];
    u32 activeMask;
    u32 hitPending;
};

#endif
