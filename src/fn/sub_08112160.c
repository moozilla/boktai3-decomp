/* Provisional names and object interpretation; offsets follow static accesses. */
// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Digit {
    u32 flags;
    u8 rest[0x5c];
};
s32 Div(s32, s32);
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
static inline u32 *FLP(struct Digit *p, s32 n)
{
    u32 o = n * 0x60;
    asm("" : "+r"(o));
    return (u32 *)((u32)p + o);
}
#define FL(p, n) (*FLP(p, n))
void StageResults_ShowTimeDigits(u8 *s, s32 mode, s32 time)
{
    struct Digit *fb;
    s32 base;
    s32 outer;
    s32 j;
    register s32 divisor asm("r5");
    u32 glyph;
    s32 seen;
    s32 idx;
    if (mode == 8) {
        s32 minutes;
        s32 hundredths;
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
                            FL(fb, mode + j) &= ~1;
                        else
                            FL(fb, mode + j) |= 1;
                    } else {
                        FL(fb, idx) &= ~1;
                        seen = 1;
                    }
                } else {
                    FL(fb, idx) &= ~1;
                }
                Anim_SelectWidgetGlyph(s + 0x24 + *(&idx) * 0x60, s + 0x8c4, (u16)glyph);
                divisor = Div(divisor, 10);
            } while (++j < 3);
        } while (++outer < 2);
    } else if (mode == 14 || mode == 17 || mode == 20) {
        divisor = 100;
        seen = 0;
        j = 0;
        fb = (struct Digit *)(s + 0x2c);
        asm("" : "+r"(fb));
        for (; j < 3; j++) {
            glyph = 0x3e;
            idx = mode + j;
            while (time >= divisor) {
                glyph++;
                time -= divisor;
            }
            if (glyph == 0x3e && j != 2) {
                if (seen)
                    FL(fb, idx) &= ~1;
                else
                    FL(fb, idx) |= 1;
            } else {
                FL(fb, idx) &= ~1;
                seen = 1;
            }
            Anim_SelectWidgetGlyph(s + 0x24 + idx * 0x60, s + 0x8c4, (u16)glyph);
            divisor = Div(divisor, 10);
        }
    }
}
