#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindow_Find(int);
const u8 *TextWindow_ResolveString(struct TextWindow *, int);
void TextState_Reset(struct TextState *);

int TextWindow_SetScript(int id, const u8 *script)
{
    struct TextWindow *window = TextWindow_Find(id);
    struct TextState *text;

    if (!window)
        return -1;
    text = &window->text;
    window->script = script;
    text->pc = TextWindow_ResolveString(window, window->index);
    window->index = 0;
    window->sequenceIndex = 0;
    window->sequenceCount = 1;
    window->symbol = 0;
    TextState_Reset(text);
    return 0;
}
