#include "ui/dialogue/message_window.h"

int MessageWindow_GetTextBounds(int *bounds)
{
    struct MessageWindow *window = (void *)gMessageWindow;

    if (!window)
        return -1;
    bounds[0] = window->state.left;
    bounds[1] = window->state.top;
    bounds[2] = window->state.width;
    bounds[3] = window->state.height;
    return 0;
}
