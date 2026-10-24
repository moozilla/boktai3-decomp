#include "global.h"
// MATCHING_CONCESSION: register-pinning

struct DigitSlot {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x60 - 0xC];
};
struct ScoreDigits {
    u8 pad0[0x24];
    struct DigitSlot slots[(0x704 - 0x24) / 0x60];
    u8 pad[0x704 - 0x24 - ((0x704 - 0x24) / 0x60) * 0x60];
    u8 glyphs[1];
};

void Anim_SelectWidgetGlyph(void *, void *, u16);

static inline void SetDigitVisible(struct ScoreDigits *p, s32 idx, s32 glyph)
{
    if (glyph == 0x3e)
        p->slots[idx].flags |= 1;
    else
        p->slots[idx].flags &= ~1;
}

// confidence=low: draws a three-digit number into a row of digit widgets, hiding leading zeros.
void sub_0810FDDC(struct ScoreDigits *p, s32 row, s32 n)
{
    s32 base = (row * 3) + 9;
    s32 glyph;
    s32 index;
    glyph = 0x3e;
    while (n > 99) {
        glyph++;
        n -= 100;
    }

    // Keep a separate visibility index and share its doubled term below.
    index = base;
    asm("" : "+r"(index) : : "r0");
    SetDigitVisible(p, index, glyph);
    Anim_SelectWidgetGlyph((u8 *)p->slots + ((index << 1) + base) * (sizeof(struct DigitSlot) / 3),
                           p->glyphs, glyph);
    glyph = 0x3e;
    {
        register s32 tens asm("r0") = base + 1;
        s32 ones;
        while (n > 9) {
            glyph++;
            n -= 10;
        }

        SetDigitVisible(p, tens, glyph);
        Anim_SelectWidgetGlyph((u8 *)&p->slots[1] +
                                   ((index << 1) + base) * (sizeof(struct DigitSlot) / 3),
                               p->glyphs, glyph);
        glyph = 0x3e;
        while (n > 0) {
            glyph++;
            n--;
        }

        ones = base + 2;
        Anim_SelectWidgetGlyph(&p->slots[ones], p->glyphs, glyph);
    }
}
