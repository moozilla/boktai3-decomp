#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindow_Find(int);
void TextWindow_OnPlayback(struct WindowManager *, struct TextWindow *);

int TextWindow_StartPlayback(int id)
{
    struct TextWindow *window = TextWindow_Find(id);

    if (!window)
        return -1;
    window->callback = TextWindow_OnPlayback;
    return 0;
}
