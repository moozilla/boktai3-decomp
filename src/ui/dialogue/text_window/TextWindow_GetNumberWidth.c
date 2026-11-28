#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"
/* TextWindow_GetNumberWidth: Returns the decimal display width of an indexed number including its sign */

struct TextWindow *TextWindow_Find(int);
int TextState_GetNumberWidth(struct TextState *, int);

int TextWindow_GetNumberWidth(int id, int index)
{
    struct TextWindow *window = TextWindow_Find(id);

    if (!window)
        return -1;
    return TextState_GetNumberWidth(&window->text, index);
}
