#include "ui/dialogue/message_window.h"

u32 TextState_MeasureTextColumns(struct MessagePlaybackState *);

u32 MessageWindow_GetRemainingTextWidth(void)
{
    struct MessageWindow *window = (void *)gMessageWindow;

    if (!window)
        return 0;
    return TextState_MeasureTextColumns(&window->state);
}
