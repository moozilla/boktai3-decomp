#include "ui/dialogue/message_window.h"

int sub_0803198C(void *, void *);

void *MessageWindow_Create(void *config)
{
    struct MessageWindow *window = (void *)gMessageWindow;

    if (!window) {
        window = Task_Create(12, sizeof(struct MessageWindow));
        if (window) {
            Task_SetCallbacks(window, MessageWindow_Update, MessageWindow_OnDestroy);
            if (sub_0803198C(window, config) < 0) {
                Task_Kill(window);
                return 0;
            }
        }
    }
    return window;
}
