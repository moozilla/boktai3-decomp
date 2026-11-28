#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"
/* TextWindow_SetNumber: Stores a value in the embedded interpreter numeric substitution array */

struct TextWindow *TextWindow_Find(int);
u32 TextState_SetNumber(struct TextState *, int, int);

int TextWindow_SetNumber(int id, int index, int number)
{
    struct TextWindow *window = TextWindow_Find(id);

    if (!window)
        return -1;
    TextState_SetNumber(&window->text, index, number);
    return 0;
}
