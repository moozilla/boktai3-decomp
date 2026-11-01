#ifndef GUARD_PINK_SPINE_PLAYBACK_H
#define GUARD_PINK_SPINE_PLAYBACK_H
#include "screamer_playback.h"
#include "player_cue_sequence.h"
struct PinkSpineState {
    u8 unknown0[24];
    u32 age;
    struct ActorPosition initialPosition;
    struct ScreamerSprite sprite;
    u8 graphics[0x1c];
    struct {
        u8 unknown0[6];
        u16 flags;
    } collider;
    u8 unknown74[76];
    u8 body[0x48];
    void *update;
    struct DisplaySequenceState sequence;
    s32 nearRadiusSquared;
    s32 farRadiusSquared;
    s16 f124;
    s16 f126;
    s16 f128;
    s16 f12A;
    s16 f12C;
    s16 f12E;
    s16 f130;
    s16 f132;
    s16 f134;
    s16 f136;
    s16 f138;
    s16 f13A;
    s16 f13C;
    s16 f13E;
    void *f140;
    u32 f144;
    u32 f148;
    u32 f14C;
    u32 f150;
    u32 f154;
    u32 f158;
    void *f15C;
};
static inline u32 PinkSpine_StepAnimation(struct PinkSpineState *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}

#endif
