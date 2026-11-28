// COMPILER: old_agbcc
/* Anim_ResetDurationScale: Restores the base frame duration and resets its scale to 64 */
#include "engine/video/display_sequence.h"

void Anim_ResetDurationScale(struct DisplaySequenceState *state)
{
    state->duration = state->frameWeight;
    state->speed = 64;
}
