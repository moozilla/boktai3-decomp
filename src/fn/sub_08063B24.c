#include "roaming_object_playback.h"
void RoamingObject_UpdateAnimation(struct RoamingObject *owner)
{
    s32 angle = (owner->angle + 32) & 255;
    s32 mask;
    s32 row;
    u32 mirror;
    struct DisplaySequenceState *sequence;
    *(&angle) >>= 6;
    angle++;
    mask = 3;
    angle &= mask;
    if (angle > 1) {
        row = mask - angle;
        mirror = 1;
    } else {
        row = angle;
        mirror = 0;
    }
    sequence = &owner->sequence;
    Anim_SetSequence(sequence, owner->sequenceResource, owner->animation, (u8)row, mirror);
    RoamingObject_StepPlayback(owner, sequence);
}
