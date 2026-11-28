#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"



struct TextWindow *TextWindowManager_FindWindow(struct WindowManager *manager, int id)
{
    struct TextWindow *window = manager->head;

    while (window) {
        struct TextWindow *previous = window->previous;

        if (window->id == id)
            return window;
        window = previous;
    }
    return 0;
}
