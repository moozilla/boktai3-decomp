#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindow_Find(int);

int TextWindow_SetPlaybackRate(int id, u32 rate)
{
    struct TextWindow *window = TextWindow_Find(id);

    if (!window)
        return -1;
    window->enabled = rate;
    if (rate > 8)
        window->enabled = 8;
}
