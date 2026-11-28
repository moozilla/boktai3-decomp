#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"
/* TextWindow_SetText: Sets the interpreter text pointer and clears script sequencing and entry state */

struct TextWindow *TextWindow_Find(int);

int TextWindow_SetText(int id, const u8 *string)
{
    struct TextWindow *window = TextWindow_Find(id);
    struct TextState *text;

    if (!window)
        return -1;
    text = &window->text;
    window->script = 0;
    window->symbol = 0;
    text->pc = string;
    text->entered = 0;
    text->style = 0;
    return 0;
}
