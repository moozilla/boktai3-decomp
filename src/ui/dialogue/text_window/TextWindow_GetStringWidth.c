#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"
/* TextWindow_GetStringWidth: Measures an indexed substitution string including expanded numbers and wide characters */

struct TextWindow *TextWindow_Find(int);
int TextState_MeasureStringLine(struct TextState *, int);

int TextWindow_GetStringWidth(int id, int index)
{
    struct TextWindow *window = TextWindow_Find(id);

    if (!window)
        return -1;
    return TextState_MeasureStringLine(&window->text, index);
}
