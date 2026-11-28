#include "ui/dialogue/message_window.h"

u32 MessageWindow_GetTextCompletionState(void)
{
    struct MessageWindow *window = (void *)gMessageWindow;

    if (!window)
        return 0;
    return window->playbackCounter;
}
