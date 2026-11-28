#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindow_Find(int);
const u8 *TextWindow_ResolveString(struct TextWindow *, int);
void TextState_Reset(struct TextState *);

int sub_080338A8(int id, const u8 *script, u32 count, u32 frame, u16 *frames)
{
    struct TextWindow *window = TextWindow_Find(id);
    struct TextState *text;
    int i;

    if (!window)
        return -1;
    text = &window->text;
    window->script = script;
    text->pc = TextWindow_ResolveString(window, 0);
    window->sequenceIndex = frame;
    window->sequenceCount = count;
    if (window->sequenceCount > 32)
        window->sequenceCount = 32;
    for (i = 0; i < window->sequenceCount; i++)
        window->sequence[i] = frames[i];
    window->symbol = window->sequence[window->sequenceIndex];
    TextState_Reset(&window->text);
    return 0;
}
