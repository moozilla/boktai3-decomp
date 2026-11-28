#include "ui/dialogue/message_window.h"

s32 TextState_SetChoiceProcedures(struct MessagePlaybackState *state, s32 count, u32 *procedures)
{
    s32 i;

    for (i = 0; i < count; i++)
        state->choices[i] = procedures[i];
    state->choiceCount = count;
}
