/* Provisional names and object interpretation; offsets follow static accesses. */
// MATCHING_CONCESSION: register-pinning
#define StageResults_ShowTimeDigits sub_08112160
#include "global.h"
struct Digit {
    u32 flags;
    u8 rest[0x5c];
};
s32 Div(s32, s32);
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
#ifdef NONMATCHING
// fuzzy: 94.7%
// Remaining: two stack slots swapped (base vs hundredths) and the digit-base
// pointer added second instead of first in the flag addresses.
void StageResults_ShowTimeDigits(u8 *s, s32 mode, s32 time)
{
    struct Digit *fb;
    s32 hundredths;
    s32 outer;
    s32 j;
    register s32 divisor asm("r5");
    u32 glyph;
    s32 seen;
    s32 idx;
    if (mode == 8) {
        s32 minutes;
        s32 clamped = time;
        if (time > 60000)
            clamped = 60000;
        else if (time < 0)
            clamped = 0;
        time = clamped;
        if (time > 59999) {
            minutes = 999;
            hundredths = 999;
        } else {
            minutes = Div(time, 60);
            hundredths = Div((time - minutes * 60) * 169, 10);
        }
        outer = 0;
        fb = (struct Digit *)(s + 0x2c);
        asm("" : "+r"(fb));
        do {
            s32 base;
            divisor = 100;
            base = outer * 3 + 8;
            time = hundredths;
            if (outer == 0)
                time = minutes;
            seen = 0;
            j = 0;
            do {
                glyph = 0x3e;
                idx = base + j;
                while (time >= divisor) {
                    glyph++;
                    time -= divisor;
                }
                if (outer == 0) {
                    if (glyph == 0x3e && j != 2) {
                        if (seen)
                            fb[mode + j].flags &= ~1;
                        else
                            fb[mode + j].flags |= 1;
                    } else {
                        fb[idx].flags &= ~1;
                        seen = 1;
                    }
                } else {
                    fb[idx].flags &= ~1;
                }
                Anim_SelectWidgetGlyph(s + 0x24 + *(&idx) * 0x60, s + 0x8c4, (u16)glyph);
                divisor = Div(divisor, 10);
            } while (++j < 3);
        } while (++outer < 2);
    } else if (mode == 14 || mode == 17 || mode == 20) {
        fb = (struct Digit *)(s + 0x2c);
        asm("" : "+r"(fb));
        divisor = 100;
        seen = 0;
        for (j = 0; j < 3; j++) {
            glyph = 0x3e;
            idx = mode + j;
            while (time >= divisor) {
                glyph++;
                time -= divisor;
            }
            if (glyph == 0x3e && j != 2) {
                if (seen)
                    fb[idx].flags &= ~1;
                else
                    fb[idx].flags |= 1;
            } else {
                fb[idx].flags &= ~1;
                seen = 1;
            }
            Anim_SelectWidgetGlyph(s + 0x24 + idx * 0x60, s + 0x8c4, (u16)glyph);
            divisor = Div(divisor, 10);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08112160);
#endif
