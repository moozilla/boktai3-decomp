#include "global.h"
#define MotionTrack_Initialize sub_081B9334
struct MotionTrackVector { s16 x, y, z, padding; };
struct MotionTrack {
    const void *data;
    u8 unknown4[8];
    struct MotionTrackVector origin, target;
    u8 unknown1C[12];
    u8 mode, active;
    u8 unknown2A[10];
    u32 cursor;
};
static inline void MotionTrack_ClearVector(void *destination)
{
    volatile u32 zero = 0;
    CpuSet((const void *)&zero, destination, 0x05000002);
}
void sub_081B9334(struct MotionTrack *track, const void *data,
                 const struct MotionTrackVector *origin, const struct MotionTrackVector *target,
                 u32 mode)
{
    if (data) {
        track->data = data;
        if (origin) track->origin = *origin;
        else {
            MotionTrack_ClearVector(&track->origin);
        }
        if (target) track->target = *target;
        else {
            MotionTrack_ClearVector(&track->target);
        }
        track->mode = mode;
        track->active = 1;
        track->cursor = 0;
    }
}
