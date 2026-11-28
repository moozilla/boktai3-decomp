#include "ui/dialogue/message_window.h"

s32 TextState_MeasureStringLine(struct MessagePlaybackState *, s32);

s32 MessageWindow_GetStringWidth(s32 string)
{
    struct MessageWindow *window = (void *)gMessageWindow;

    if (!window)
        return 0;
    return TextState_MeasureStringLine(&window->state, string);
}
