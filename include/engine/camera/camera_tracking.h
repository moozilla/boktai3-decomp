#ifndef GUARD_CAMERA_TRACKING_H
#define GUARD_CAMERA_TRACKING_H

#include "engine/shared/render.h"

struct CameraTracking {
    struct CameraPosition *first, *second;
    s32 scale, scaleStep, minimum, maximum, divisor;
    u32 flags, requests;
    s32 active, blend, followMode;
};

static inline void CameraTracking_SetRange(struct CameraTracking *tracking, s32 minimum, s32 maximum)
{
    tracking->minimum = minimum;
    tracking->maximum = maximum;
}

void CameraTracking_Update(struct CameraTracking *tracking);

/* Request bits an actor raises for the tracking update (bit 0: keep the actor in frame). */
static inline void CameraTracking_SetRequests(struct CameraTracking *tracking, u32 mask)
{
    tracking->requests |= mask;
}

static inline void CameraTracking_ClearRequests(struct CameraTracking *tracking, u32 mask)
{
    tracking->requests &= ~mask;
}

#endif
