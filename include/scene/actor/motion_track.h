#ifndef GUARD_MOTION_TRACK_H
#define GUARD_MOTION_TRACK_H

#include "global.h"

/* Shared by MotionTrack_Initialize and the track embedded in Vanargand.
 * Unknown bytes remain opaque; the names describe the observed operations. */
struct MotionTrackVector {
    s16 x, y, z, padding;
};

struct MotionTrack {
    const void *data;
    u8 unknown4[8];
    struct MotionTrackVector origin, target;
    u8 unknown1C[12];
    u8 mode, active;
    u8 unknown2A[10];
    u32 cursor;
};

static inline void MotionTrack_Reset(struct MotionTrack *track)
{
    track->data = 0;
    track->active = 0;
    track->mode = 0;
    track->cursor = 0;
}

void MotionTrack_Initialize(struct MotionTrack *track, const void *data,
                            const struct MotionTrackVector *origin,
                            const struct MotionTrackVector *target, u32 mode);

#endif
