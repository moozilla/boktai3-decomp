#include "global.h"
struct TextState {
    u8 x, y, left, top, width, height, palette, style;
    u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice, toggle, lastWidth,
        pad14;
    s8 current;
    u8 instant, wrap;
    u32 frame;
    const u8 *pc, *streams[4];
    u8 pad30[0xD8];
    u8 depth, styles[7];
};
// MATCHING_CONCESSION: register-pinning
const u8 *TextState_ProcessTag(struct TextState *, const u8 *);
u16 Text_DecodeGlyphEscape(const u8 *);
int TextState_DrawNarrowGlyph(struct TextState *, u32);
int TextState_DrawWideGlyph(struct TextState *, u32);
int TextState_AdvanceCursor(struct TextState *, int);
void TextState_AdvanceLine(struct TextState *);
u32 Text_GetWideGlyphCount(void);
const u8 *TextState_RenderNextGlyph(struct TextState *p, const u8 *s)
{
    u32 c;
    u16 glyph;
    goto test;
tag:
    s = TextState_ProcessTag(p, s);
    if (p->padE)
        goto done;
    if (*s)
        goto next;
    goto done;
plain:
    asm("" ::: "memory");
    c = *s;
    if (c == 10) {
        TextState_AdvanceLine(p);
        goto next;
    }
    if (c == 31) {
        u32 code = (u16)Text_DecodeGlyphEscape(s);
        if (code == 0xFFFF) {
            glyph = *s - 32;
            TextState_DrawNarrowGlyph(p, glyph);
        } else if (code & 0xFF00) {
            u32 mask = 0x7FFF;
            code &= mask;
            TextState_DrawWideGlyph(p, code);
        } else
            TextState_DrawNarrowGlyph(p, code);
        s += 2;
        goto done;
    }
    asm("" ::: "memory");
    glyph = *s - 32;
    TextState_DrawNarrowGlyph(p, glyph);
    s++;
    goto done;
wide: {
    register int low asm("r1");
    asm("" ::: "memory");
    low = *s;
    {
        u32 mask = 0x7F;
        c = mask & low;
    }
    s++;
    low = *s;
    glyph = (c << 8) | low;
}
    if (glyph >= Text_GetWideGlyphCount())
        TextState_AdvanceCursor(p, 1);
    else
        TextState_DrawWideGlyph(p, glyph);
    s++;
    goto done;
next:
    s++;
test:
    c = *s;
    c <<= 24;
    if ((s32)c < 0)
        goto wide;
    c >>= 24;
    if (!c)
        goto done;
    if (c != '<')
        goto plain;
    s++;
    if (*s)
        goto tag;
done:
    return s;
}
