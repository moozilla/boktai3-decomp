#include "global.h"
#include "ui/dialogue/text_state.h"
const u8 *TextState_RenderNextGlyph(struct TextState *, const u8 *);
void TextState_PopStream(struct TextState *);
int TextState_StepPlayback(struct TextState *p)
{
    const u8 *s;
    int result;
    int c;
    p->padE = 0;
    switch (p->style) {
        case 0:
            s = TextState_RenderNextGlyph(p, p->pc);
            p->pc = s;
            if (p->fresh)
                goto yes;
            if (p->style)
                goto no;
            c = *s;
            goto endchar;
        case 1:
            s = TextState_RenderNextGlyph(p, p->streams[0]);
            p->streams[0] = s;
            if (p->style != 1)
                goto no;
            goto stream;
        case 2:
            s = TextState_RenderNextGlyph(p, p->streams[1]);
            p->streams[1] = s;
            if (p->style != 2)
                goto no;
            goto stream;
        case 3:
            s = TextState_RenderNextGlyph(p, p->streams[2]);
            p->streams[2] = s;
            if (p->style != 3)
                goto no;
            goto stream;
        case 4:
            s = TextState_RenderNextGlyph(p, p->streams[3]);
            p->streams[3] = s;
            if (p->style != 4)
                goto no;
        stream:
            if (*s)
                goto no;
            TextState_PopStream(p);
            if (p->style)
                goto no;
            if (p->fresh)
                goto yes;
            c = *p->pc;
        endchar:
            if (c)
                goto no;
            goto yes;
        default:
            goto no;
    }
yes:
    result = 1;
    goto done;
no:
    result = 0;
done:
    return result;
}
