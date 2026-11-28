#include "ui/dialogue/message_window.h"

int TextState_GetNumberWidth(struct MessagePlaybackState *state, int index)
{
    int value = state->numbers[index];
    int width = (u32)value >> 31;
    int magnitude = value;

    if (value < 0)
        magnitude = -value;
    value = magnitude;
    if (value > 9999999)
        width += 8;
    else if (value > 999999)
        width += 7;
    else if (value > 99999)
        width += 6;
    else if (value > 9999)
        width += 5;
    else if (value > 999)
        width += 4;
    else if (value > 99)
        width += 3;
    else if (value > 9)
        width += 2;
    else
        width++;
    return width;
}
