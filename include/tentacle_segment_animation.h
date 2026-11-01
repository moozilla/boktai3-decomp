#ifndef GUARD_TENTACLE_SEGMENT_ANIMATION_H
#define GUARD_TENTACLE_SEGMENT_ANIMATION_H

#include "display_sequence.h"

struct TentacleSegmentAnimation {
    struct DisplaySequenceState sequence;
    void *script;
    u8 direction, status;
};

struct TentacleAnimationSegment {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[0xDE];
    struct TentacleSegmentAnimation animation;
    u8 unknown108[0x20];
};

typedef char TentacleAnimationSegmentSize[(sizeof(struct TentacleAnimationSegment) == 0x128) ? 1 : -1];

static inline u8 TentacleSegment_HasStatus(struct TentacleSegmentAnimation *animation, u8 mask)
{
    if (mask & animation->status)
        return 1;
    return 0;
}

#endif
