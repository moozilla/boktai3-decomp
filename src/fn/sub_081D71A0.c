#include "global.h"
void TextSequence_SetState(u8 *, u32);
void TextSequence_UpdateHold(u8 *p, s16 *lim)
{
    s16 *c = (s16 *)(p + 0x322);
    (*c)++;
    if (*c >= *lim)
        TextSequence_SetState(p, 2);
}
