#include "global.h"
#define FieldHud_UpdateTimeDigits sub_081793EC
s32 Div(s32, s32);
void Anim_SelectWidgetGlyph(void *, void *, u16);
void FieldHud_UpdateTimeDigits(u8 *p)
{
    s32 **source = (s32 **)(p + 0x3a8);
    s32 minutes, fraction, digit;
    if (!*source)
        return;
    minutes = Div(**source, 60);
    fraction = Div((**source - minutes * 60) * 169, 10);
    if (**source > 59999) {
        Anim_SelectWidgetGlyph(p + 0x144, p + 0x384, 50);
        Anim_SelectWidgetGlyph(p + 0x1a4, p + 0x384, 50);
        Anim_SelectWidgetGlyph(p + 0x204, p + 0x384, 50);
        Anim_SelectWidgetGlyph(p + 0x264, p + 0x384, 50);
        Anim_SelectWidgetGlyph(p + 0x2c4, p + 0x384, 50);
        Anim_SelectWidgetGlyph(p + 0x324, p + 0x384, 50);
    } else {
        digit = 0;
        while (minutes > 99) {
            digit++;
            minutes -= 100;
        }
        Anim_SelectWidgetGlyph(p + 0x144, p + 0x384, (u16)(digit + 41));
        digit = 0;
        while (minutes > 9) {
            digit++;
            minutes -= 10;
        }
        Anim_SelectWidgetGlyph(p + 0x1a4, p + 0x384, (u16)(digit + 41));
        Anim_SelectWidgetGlyph(p + 0x204, p + 0x384, (u16)(minutes + 41));
        digit = 0;
        while (fraction > 99) {
            digit++;
            fraction -= 100;
        }
        Anim_SelectWidgetGlyph(p + 0x264, p + 0x384, (u16)(digit + 41));
        digit = 0;
        while (fraction > 9) {
            digit++;
            fraction -= 10;
        }
        Anim_SelectWidgetGlyph(p + 0x2c4, p + 0x384, (u16)(digit + 41));
        Anim_SelectWidgetGlyph(p + 0x324, p + 0x384, (u16)(fraction + 41));
    }
}
