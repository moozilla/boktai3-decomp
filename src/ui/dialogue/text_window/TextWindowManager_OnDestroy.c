#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

int TextWindow_Destroy(int);

int TextWindowManager_OnDestroy(struct WindowManager *manager)
{
    struct TextWindow *window = manager->head;

    while (window) {
        struct TextWindow *previous = window->previous;

        TextWindow_Destroy(window->id);
        window = previous;
    }
    gWindowManager = 0;
    return 0;
}
