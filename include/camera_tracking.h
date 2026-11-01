#ifndef GUARD_CAMERA_TRACKING_H
#define GUARD_CAMERA_TRACKING_H

#include "render.h"

struct CameraTracking {
    struct CameraPosition *first, *second;
    s32 scale, scaleStep, minimum, maximum, divisor;
    u32 flags, requests;
    s32 active, blend, followMode;
};

void CameraTracking_Update(struct CameraTracking *tracking);

#endif
