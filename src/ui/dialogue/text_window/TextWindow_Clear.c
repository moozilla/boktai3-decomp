#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindow_Find(int);
void TextWindow_BlankRect(int, int, int, int);
void TextWindow_OnIdle(struct WindowManager *, struct TextWindow *);

int TextWindow_Clear(int id)
{
    struct TextWindow *window = TextWindow_Find(id);

    if (!window)
        return -1;
    TextWindow_BlankRect(window->x, window->y, window->width, window->height);
    window->callback = TextWindow_OnIdle;
    return 0;
}
