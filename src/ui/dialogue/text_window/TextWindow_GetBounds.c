#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"
/* TextWindow_GetBounds: Returns the embedded text bounds as left top width and height */

struct TextWindow *TextWindow_Find(int);

int TextWindow_GetBounds(int id, int *bounds)
{
    struct TextWindow *window = TextWindow_Find(id);

    struct TextState *text;

    if (!window)
        return -1;
    text = &window->text;
    bounds[0] = text->left;
    bounds[1] = text->top;
    bounds[2] = text->width;
    bounds[3] = text->height;
    return 0;
}
