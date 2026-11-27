#include "global.h"
#include "ui/dialogue/text_state.h"
int TextState_AdvanceCursor(struct TextState *p, int wide)
{
    if (p->wrap != 2)
        p->wrap = 0;
    if (!wide)
        p->x++;
    else
        p->x += 2;
}
