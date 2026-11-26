#ifndef GUARD_DIRECTIONAL_FORCE_H
#define GUARD_DIRECTIONAL_FORCE_H
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
struct DirectionalForce {
    struct Task task;
    u16 id, mode;
    u16 baseMagnitude, magnitudeRange, baseDelay, delayRange;
    u16 magnitude, targetMagnitude, angle, targetAngle, delay, unknown2E;
};
/* 0x02000114 */
extern struct DirectionalForce *gDirectionalForce;
s32 Mod(s32, s32);
#endif
