#ifndef GUARD_CAMERA_TRACKING_H
#define GUARD_CAMERA_TRACKING_H

#include "render.h"

struct CameraTracking {
    struct CameraPosition *first, *second;
    s32 scale, scaleStep, minimum, maximum, divisor;
    u32 flags, requests;
    s32 active, blend, followMode;
};

static inline void CameraTracking_ClearRequests(struct CameraTracking *tracking, u32 requests)
{
    tracking->requests &= ~requests;
}

static inline void CameraTracking_SetRange(struct CameraTracking *tracking, s32 minimum, s32 maximum)
{
    tracking->minimum = minimum;
    tracking->maximum = maximum;
}

void CameraTracking_Update(struct CameraTracking *tracking);

#endif
