#include "global.h"
#include "ui/dialogue/text_state.h"
void TextState_PushStream(struct TextState *p, int style)
{
    u8 *depth = &p->depth;
    if (*depth <= 6) {
        p->styles[*depth] = p->style;
        (*depth)++;
        p->style = style;
    }
}
