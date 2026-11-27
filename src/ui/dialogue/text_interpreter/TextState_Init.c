#include "global.h"
#include "ui/dialogue/text_state.h"
void TextState_SetBounds(struct TextState *, int, int, int, int);
void TextState_Reset(struct TextState *);
void TextState_ResetPlaybackMode(struct TextState *);
void TextState_Init(struct TextState *p, int x, int y, int w, int h)
{
    TextState_SetBounds(p, x, y, w, h);
    TextState_Reset(p);
    TextState_ResetPlaybackMode(p);
    p->skip = 0;
    p->busy = 0;
    p->padC = p->mode;
    p->lastWidth = 0;
    p->choiceCount = 0;
    p->pc = 0;
    p->streams[0] = 0;
    p->streams[1] = 0;
    p->streams[2] = 0;
    p->streams[3] = 0;
    p->current = -1;
    p->pad14 = 1;
    p->frame = 0;
    p->wrap = 0;
}
