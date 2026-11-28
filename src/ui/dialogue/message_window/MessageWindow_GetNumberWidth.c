#include "ui/dialogue/message_window.h"

s32 TextState_GetNumberWidth(struct MessagePlaybackState *, s32);

s32 MessageWindow_GetNumberWidth(s32 number)
{
    struct MessageWindow *window = (void *)gMessageWindow;

    if (!window)
        return 0;
    return TextState_GetNumberWidth(&window->state, number);
}
