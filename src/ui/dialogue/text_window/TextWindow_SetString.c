#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindow_Find(int);
int TextState_SetString(struct TextState *, int, int);

int TextWindow_SetString(int id, int index, int string)
{
    struct TextWindow *window = TextWindow_Find(id);

    if (window)
        return TextState_SetString(&window->text, index, string);
    return -1;
}
