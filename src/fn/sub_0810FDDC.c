#include "scene_input.h"

struct ScoreDigits {
    u8 taskAndResources[0x24];
    struct SceneInputWidget slots[18];
    u8 unknown6E4[0x20];
    struct SceneInputResource glyphs;
};

// Returns the next slot index.
static inline s32 SetDigitVisible(struct ScoreDigits *p, s32 idx, s32 glyph)
{
    if (glyph == 0x3e)
        p->slots[idx].flags |= 1;
    else
        p->slots[idx].flags &= ~1;
    return ++idx;
}

// Slots 9 through 17 hold the three rows of digits positioned by sub_0810FADC.
// `row` is reused as the hundreds slot; its dead final increment keeps that copy
// separate from `base` (cse keeps the longer-lived register as the class head).
void sub_0810FDDC(struct ScoreDigits *p, s32 row, s32 n)
{
    s32 base = (row * 3) + 9;
    s32 glyph;
    s32 ones;
    glyph = 0x3e;
    while (n > 99) {
        glyph++;
        n -= 100;
    }
    row = base;
    SetDigitVisible(p, row, glyph);
    Anim_SelectWidgetGlyph(&p->slots[base], &p->glyphs, (u16)glyph);
    glyph = 0x3e;
    while (n > 9) {
        glyph++;
        n -= 10;
    }
    SetDigitVisible(p, base + 1, glyph);
    Anim_SelectWidgetGlyph(&p->slots[base + 1], &p->glyphs, (u16)glyph);
    glyph = 0x3e;
    while (n > 0) {
        glyph++;
        n--;
    }
    ones = base + 2;
    Anim_SelectWidgetGlyph(&p->slots[ones], &p->glyphs, (u16)glyph);
    row++;
}
