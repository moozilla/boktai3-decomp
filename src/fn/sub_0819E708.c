#include "global.h"
#define ActorAnimation_ResetSequence  sub_0819E708
#define gActorAnimationSequenceValues gUnk_08611D44
extern const u32 gUnk_08611D44[];
struct ActorAnimationSequence {
    u8 unknown0[0xAC];
    u8 frame, elapsed, duration, unknownAF, active;
    u8 unknownB1[0x13];
    u32 accumulator;
    u8 unknownC8[0x264];
    u16 sequence, step;
    u8 unknown330[0x58];
    u8 state;
};
void sub_0819E708(struct ActorAnimationSequence *actor)
{
    u32 value;
    actor->sequence = 0;
    actor->step = 0;
    value = gActorAnimationSequenceValues[actor->step];
    actor->frame = 0;
    actor->elapsed = 0;
    actor->duration = value;
    actor->accumulator = 0;
    actor->active = 1;
    actor->state = 0;
}
