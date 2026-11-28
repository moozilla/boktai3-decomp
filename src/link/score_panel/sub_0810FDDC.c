#include "ui/widgets/scene_input.h"
#include "ui/save/save_menu.h"

// Returns the next slot index.
static inline s32 SetDigitVisible(struct SaveMenu *p, s32 idx, s32 glyph)
{
    if (glyph == 0x3e)
        p->widgets[idx].flags |= 1;
    else
        p->widgets[idx].flags &= ~1;
    return ++idx;
}

// Slots 9 through 17 hold the three rows of digits positioned by sub_0810FADC.
// `row` is reused as the hundreds slot; its dead final increment keeps that copy
// separate from `base` (cse keeps the longer-lived register as the class head).
void sub_0810FDDC(struct SaveMenu *p, s32 row, s32 n)
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
    AnimSprite_SelectGlyph(&p->widgets[base], &p->widgetResource, (u16)glyph);
    glyph = 0x3e;
    while (n > 9) {
        glyph++;
        n -= 10;
    }
    SetDigitVisible(p, base + 1, glyph);
    AnimSprite_SelectGlyph(&p->widgets[base + 1], &p->widgetResource, (u16)glyph);
    glyph = 0x3e;
    while (n > 0) {
        glyph++;
        n--;
    }
    ones = base + 2;
    AnimSprite_SelectGlyph(&p->widgets[ones], &p->widgetResource, (u16)glyph);
    row++;
}
