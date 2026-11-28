#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindow_Find(int);
void TextWindow_BlankRect(int, int, int, int);
int TextState_SetBounds(struct TextState *, int, int, int, int);

int TextWindow_SetBounds(int id, int x, int y, int width, int height)
{
    struct TextWindow *window = TextWindow_Find(id);

    if (window) {
        struct TextState *text = &window->text;

        TextWindow_BlankRect(window->x, window->y, window->width, window->height);
        window->x = x;
        window->y = y;
        window->width = width;
        window->height = height;
        return TextState_SetBounds(text, window->x, window->y, window->width, window->height);
    }
    return -1;
}
