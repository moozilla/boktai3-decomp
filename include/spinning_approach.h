#ifndef GUARD_SPINNING_APPROACH_H
#define GUARD_SPINNING_APPROACH_H

#include "motion_0818.h"
#include "engine.h"

/* The shared motion record also serves neighboring return and damage callbacks.
 * a/b/c hold a correction vector, then an owner-position snapshot; fx/fy/fz
 * retain the initial owner-relative offset. byte1DD is the signed spin step.
 */
typedef struct Motion0818Object SpinningApproach;

struct SpinningApproachOwner {
    u8 unknown0[0x5AC];
    u16 x, y, z;
    u8 unknown5B2[0x10];
    u8 flags;
};

void SpinningApproach_InitMotion(SpinningApproach *);
void SpinningApproach_UpdateSpinRamp(SpinningApproach *);
void SpinningApproach_UpdateApproach(SpinningApproach *);
void SpinningApproach_UpdateOwnerRelativeSegment(SpinningApproach *);

#endif
