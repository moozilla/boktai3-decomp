#ifndef GUARD_TENTACLE_SEGMENT_ANIMATION_H
#define GUARD_TENTACLE_SEGMENT_ANIMATION_H

#include "display_sequence.h"
#include "contact_position.h"

struct TentacleSegmentAnimation {
    struct DisplaySequenceState sequence;
    void *script;
    u8 direction, status;
};

struct TentacleAnimationSegment {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[0x1C - 0x12];
    struct ContactPosition position;
    u8 unknown24[0xF0 - 0x24];
    struct TentacleSegmentAnimation animation;
    u8 unknown108[0x11C - 0x108];
    struct ContactPosition offset;
    u8 unknown124[4];
};

typedef char
    TentacleAnimationSegmentSize[(sizeof(struct TentacleAnimationSegment) == 0x128) ? 1 : -1];

static inline u8 TentacleSegment_HasStatus(struct TentacleSegmentAnimation *animation, u8 mask)
{
    if (mask & animation->status)
        return 1;
    return 0;
}

struct TentacleBodyAnimationView {
    u8 unknown0[0x2C2];
    u16 speed;
    u32 profileMode;
    u8 unknown2C8[0x308 - 0x2C8];
    u32 profileId;
    u8 unknown30C[0x3D4 - 0x30C];
    struct TentacleAnimationSegment segments[4];
    u8 unknown874;
    u8 spin;
    u8 unknown876[0x8DA - 0x876];
    s16 segmentCount;
};

static inline void *TentacleSegment_ScriptAt(struct TentacleBodyAnimationView *body, s32 offset)
{
    void **script = &body->segments[0].animation.script;
    return *(void **)((u8 *)script + offset);
}

#endif
