#include "global.h"
#include "ui/dialogue/text_state.h"
void TextState_PlayGlyphSound(struct TextState *, int);
void TextState_AdvanceLine(struct TextState *);
int TextState_AdvanceCursor(struct TextState *, int);
void TextWindow_DrawWideGlyph(int, int, int, int);
int TextState_DrawWideGlyph(struct TextState *p, u16 c)
{
    TextState_PlayGlyphSound(p, c);
    if (p->wrap != 2 && p->x >= p->left + p->width - 1) {
        TextState_AdvanceLine(p);
        p->wrap = 1;
    }
    TextWindow_DrawWideGlyph(c, p->x, p->y, p->palette);
    TextState_AdvanceCursor(p, 1);
    p->lastWidth = 1;
}
