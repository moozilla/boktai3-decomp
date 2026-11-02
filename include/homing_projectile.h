#ifndef GUARD_HOMING_PROJECTILE_H
#define GUARD_HOMING_PROJECTILE_H

#include "contact_position.h"
#include "display_sequence.h"

struct HomingSlot;
typedef s32 (*HomingBehavior)(struct HomingSlot *, s32);

struct HomingParams {
    u16 unknown0, delay;
    u8 unknown4[2];
    s8 angle;
    u8 variant;
    s32 speed;
    HomingBehavior behavior;
    s16 attack, element, attackFlags, body;
    s16 shotA, shotB, shotC;
    u8 unknown1E[2];
};

struct HomingCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x4c];
};

struct HomingSprite {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xa];
    struct ContactPosition position;
    u8 unknown24[8];
};

struct HomingGraphics {
    u8 unknown0[6];
    u16 paletteMode;
    u8 unknown8[4];
    void *palette;
    u8 unknown10[0xc];
    void *resource;
};

struct HomingSlot {
    u32 lifetime;
    u8 unknown04[8];
    struct ContactPosition velocity, origin, target;
    struct HomingCollider secondaryCollider, primaryCollider;
    struct HomingGraphics graphics;
    struct HomingParams params;
    struct HomingSprite sprite;
    u32 motionFlags;
    s32 state;
    struct DisplaySequenceState sequence;
    u32 slotIndex;
    u32 paletteModeCopy;
    u8 unknown158[4];
    s16 flashTimer, blinkTimer;
};

s32 HomingProjectile_UpdateDefaultState(struct HomingSlot *, s32);
s32 HomingProjectile_UpdateDefaultBehavior(struct HomingSlot *, s32);
void HomingProjectile_SetBehaviorCallback(struct HomingParams *, HomingBehavior);
void HomingProjectile_InitParams(struct HomingParams *);

typedef char HomingParamsSizeCheck[sizeof(struct HomingParams) == 0x20 ? 1 : -1];
typedef char HomingSlotSizeCheck[sizeof(struct HomingSlot) == 0x160 ? 1 : -1];

#endif
