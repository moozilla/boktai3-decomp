#include "global.h"
#include "motion_track.h"
static inline void MotionTrack_ClearVector(void *destination)
{
    volatile u32 zero = 0;
    CpuSet((const void *)&zero, destination, 0x05000002);
}
void MotionTrack_Initialize(struct MotionTrack *track, const void *data,
                            const struct MotionTrackVector *origin,
                            const struct MotionTrackVector *target, u32 mode)
{
    if (data) {
        track->data = data;
        if (origin)
            track->origin = *origin;
        else {
            MotionTrack_ClearVector(&track->origin);
        }
        if (target)
            track->target = *target;
        else {
            MotionTrack_ClearVector(&track->target);
        }
        track->mode = mode;
        track->active = 1;
        track->cursor = 0;
    }
}
