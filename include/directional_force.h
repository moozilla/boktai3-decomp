#ifndef GUARD_DIRECTIONAL_FORCE_H
#define GUARD_DIRECTIONAL_FORCE_H
#include "engine.h"
#include "ram.h"
struct DirectionalForce {
    u8 unknown0[0x18];
    u16 id, mode;
    u16 baseMagnitude, magnitudeRange, baseDelay, delayRange;
    u16 magnitude, targetMagnitude, angle, targetAngle, delay, unknown2E;
};
extern struct DirectionalForce *gDirectionalForce;
extern u16 gPlayerBackgroundState;
extern u32 gRandomIndex;
s32 Mod(s32, s32);
#endif
