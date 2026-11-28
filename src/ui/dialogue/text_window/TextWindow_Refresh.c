#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindow_Find(int);
const u8 *TextWindow_ResolveString(struct TextWindow *, int);
void TextState_Reset(struct TextState *);
void TextWindow_BlankRect(int, int, int, int);
void TextState_SetBounds(struct TextState *, int, int, int, int);
void TextWindow_OnPlayback(struct WindowManager *, struct TextWindow *);

int TextWindow_Refresh(int id, int index)
{
    struct TextWindow *window = TextWindow_Find(id);
    struct TextState *text;

    if (!window)
        return -1;
    text = &window->text;
    if (window->script) {
        TextWindow_BlankRect(window->x, window->y, window->width, window->height);
        TextState_SetBounds(text, window->x, window->y, window->width, window->height);
        window->index = index;
        text->pc = TextWindow_ResolveString(window, window->index);
        window->callback = TextWindow_OnPlayback;
        TextState_Reset(text);
        window->symbol = 0xFFFF;
    }
    return 0;
}
