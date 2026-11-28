#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

int TextWindow_Destroy(int);

void TextWindowManager_DestroyAllWindows(void)
{
    struct WindowManager *manager = (void *)gWindowManager;

    if (manager) {
        struct TextWindow *window = manager->head;

        while (window) {
            struct TextWindow *previous = window->previous;

            TextWindow_Destroy(window->id);
            window = previous;
        }
    }
}
