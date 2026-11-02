#include "global.h"
#include "motion_track.h"
void MotionTrack_Initialize(struct MotionTrack *track, const void *data,
                            const struct MotionTrackVector *origin,
                            const struct MotionTrackVector *target, u32 mode)
{
    if (data) {
        track->data = data;
        if (origin)
            track->origin = *origin;
        else {
            ZeroFill8(&track->origin);
        }
        if (target)
            track->target = *target;
        else {
            ZeroFill8(&track->target);
        }
        track->mode = mode;
        track->active = 1;
        track->cursor = 0;
    }
}
