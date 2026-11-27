#include "global.h"
#include "ui/dialogue/text_state.h"
void TextState_AdvanceLine(struct TextState *p)
{
    if (p->wrap == 1)
        goto clear;
    if (p->y - p->top < p->height) {
        p->x = p->left;
        p->y += 2;
    }
    return;
clear:
    p->wrap = 0;
}
