#include "global.h"
#include "ui/dialogue/text_state.h"
void TextState_PopStream(struct TextState *p)
{
    u8 *depth = &p->depth;
    if (*depth) {
        (*depth)--;
        p->style = p->styles[*depth];
    }
}
