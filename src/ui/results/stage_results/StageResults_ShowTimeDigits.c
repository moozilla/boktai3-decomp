#include "global.h"
#include "ui/results/stage_results.h"

s32 Div(s32, s32);
s32 AnimSprite_SelectGlyph(void *, void *, u32);

void StageResults_ShowTimeDigits(struct StageResults *results, s32 field, s32 value)
{
    s32 row;
    s32 column;
    s32 base;
    s32 index;
    s32 divisor;
    u32 glyph;
    s32 shown;
    if (field == 8) {
        s32 minutes;
        s32 hundredths;
        value = StageResults_Clamp(value, 60000);
        if (value > 59999) {
            minutes = 999;
            hundredths = 999;
        } else {
            minutes = Div(value, 60);
            hundredths = Div((value - minutes * 60) * 169, 10);
        }
        for (row = 0; row < 2; row++) {
            divisor = 100;
            base = row * 3 + 8;
            value = hundredths;
            if (row == 0)
                value = minutes;
            shown = 0;
            for (column = 0; column < 3; column++) {
                glyph = 0x3e;
                index = base + column;
                while (value >= divisor) {
                    glyph++;
                    value -= divisor;
                }
                if (row == 0) {
                    if (glyph == 0x3e && column != 2) {
                        if (shown)
                            results->sprites[field + column].flags &= ~1;
                        else
                            results->sprites[field + column].flags |= 1;
                    } else {
                        results->sprites[index].flags &= ~1;
                        shown = 1;
                    }
                } else {
                    results->sprites[index].flags &= ~1;
                }
                /* *&index keeps the digit index in memory */
                AnimSprite_SelectGlyph(&results->sprites[*&index], results->animation, (u16)glyph);
                divisor = Div(divisor, 10);
            }
        }
    } else if (field == 14 || field == 17 || field == 20) {
        divisor = 100;
        shown = 0;
        for (column = 0; column < 3; column++) {
            glyph = 0x3e;
            index = field + column;
            while (value >= divisor) {
                glyph++;
                value -= divisor;
            }
            if (glyph == 0x3e && column != 2) {
                if (shown)
                    results->sprites[index].flags &= ~1;
                else
                    results->sprites[index].flags |= 1;
            } else {
                results->sprites[index].flags &= ~1;
                shown = 1;
            }
            AnimSprite_SelectGlyph(&results->sprites[index], results->animation, (u16)glyph);
            divisor = Div(divisor, 10);
        }
    }
}
