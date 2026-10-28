#include "global.h"
#include "ram.h"
struct TextState {
    u8 x, y, left, top, width, height, palette, style;
    u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice, toggle, lastWidth,
        pad14;
    s8 current;
    u8 instant, wrap;
    u8 pad18[4];
    const u8 *pc;
    u8 pad20[0xE8];
    u8 depth, styles[7];
};
extern const s16 gUnk_086055E0[];
void Sound_PlaySong(int);
void TextState_PlayGlyphSound(struct TextState *p, int c)
{
    if (p->skip)
        goto done;
    if (p->busy)
        goto done;
    if (p->mode <= 1 || p->instant) {
        if (p->toggle)
            goto clear;
    }
    if (c && c != gSilentGlyph)
        Sound_PlaySong(gUnk_086055E0[p->voice]);
    if (!p->toggle)
        goto set;
clear:
    p->toggle = 0;
    return;
set:
    p->toggle++;
done:;
}
