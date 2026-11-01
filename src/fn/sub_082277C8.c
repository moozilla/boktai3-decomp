#define Camera_SetFollowMode Camera_ResetOffset
#include "camera_tracking.h"
s32 Camera_CalculateDistanceScale(struct CameraTracking *);
void CameraTracking_UpdatePosition(struct CameraTracking *);
void Camera_ResetOffset(s32);
static inline u32 testRequest(u32 *requests, u32 mask)
{
    return *requests & mask;
}
void CameraTracking_Update(struct CameraTracking *tracking)
{
    if (testRequest(&tracking->requests, 1)) {
        if (!tracking->active) {
            tracking->active = 1;
            tracking->scale = Camera_CalculateDistanceScale(tracking);
            tracking->blend = 0;
        }
        CameraTracking_UpdatePosition(tracking);
        if (tracking->blend < 128)
            tracking->blend++;
    } else {
        tracking->active = 0;
        Camera_ResetOffset(tracking->followMode);
    }
    tracking->requests = 0;
}
