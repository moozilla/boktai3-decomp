#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

int Script_ParseStringRef(const void *);
const u8 *Text_LookupString(int);

const u8 *TextWindow_ResolveString(struct TextWindow *window, int index)
{
    return Text_LookupString(Script_ParseStringRef(window->script) + index);
}
