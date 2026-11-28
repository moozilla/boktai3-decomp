#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

void Heap_Free(void *);
void TextWindow_BlankRect(int, int, int, int);
void TextWindow_OnIdle(struct WindowManager *, struct TextWindow *);
struct TextWindow *TextWindowManager_FindWindow(struct WindowManager *, int);

static inline void TextWindow_Unlink(struct WindowManager *manager, struct TextWindow *window)
{
    if (window->next)
        window->next->previous = window->previous;
    else
        manager->head = window->previous;
    if (window->previous)
        window->previous->next = window->next;
    window->next = 0;
    window->previous = 0;
}

int TextWindow_Destroy(int id)
{
    struct WindowManager *manager = (void *)gWindowManager;
    struct TextWindow *window;

    if (!manager || !(window = TextWindowManager_FindWindow(manager, id)))
        return -1;
    TextWindow_BlankRect(window->x, window->y, window->width, window->height);
    window->callback = TextWindow_OnIdle;
    TextWindow_Unlink(manager, window);
    Heap_Free(window);
    return id;
}
