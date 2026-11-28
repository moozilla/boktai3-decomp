#include "engine/video/display_sequence.h"

u32 Anim_SetSequence(struct DisplaySequenceState *, u8 *, u16, u8, u8);

u32 Anim_RestartSequence(struct DisplaySequenceState *state, u8 *data, u16 sequence, u8 row,
                         u8 flags)
{
    state->frameCount = 0;
    return Anim_SetSequence(state, data, sequence, row, flags);
}
