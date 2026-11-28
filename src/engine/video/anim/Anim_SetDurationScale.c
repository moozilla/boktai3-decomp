#include "engine/video/display_sequence.h"

void Anim_SetDurationScale(struct DisplaySequenceState *state, u16 speed)
{
    if (state->speed != speed) {
        state->duration = (state->frameWeight * speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
        state->speed = speed;
    }
}
