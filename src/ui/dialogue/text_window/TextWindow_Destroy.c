#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

void Heap_Free(void *);
void TextWindow_BlankRect(int, int, int, int);
void TextWindow_OnIdle(struct WindowManager *, struct TextWindow *);
struct TextWindow *TextWindowManager_FindWindow(struct WindowManager *, int);

int TextWindow_Destroy(int id)
{
    struct WindowManager *manager = (void *)gWindowManager;
    struct TextWindow *window;
    struct TextWindow *previous;
    struct TextWindow **nextSlot;
    struct TextWindow **previousSlot;
    struct TextWindow **link;

    if (!manager || !(window = TextWindowManager_FindWindow(manager, id)))
        return -1;
    TextWindow_BlankRect(window->x, window->y, window->width, window->height);
    window->callback = TextWindow_OnIdle;
    link = (struct TextWindow **)window->next;
    nextSlot = &window->next;
    if (link) {
        link = &((struct TextWindow *)link)->previous;
        previousSlot = &window->previous;
        *link = *previousSlot;
    } else {
        previousSlot = &window->previous;
        manager->head = *previousSlot;
    }
    link = previousSlot;
    previous = *link;
    if (previous)
        previous->next = *nextSlot;
    *nextSlot = NULL;
    *link = NULL;
    Heap_Free(window);
    return id;
}
