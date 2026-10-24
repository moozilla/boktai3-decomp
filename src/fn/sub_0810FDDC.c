#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Open: register allocation. The original keeps p in r7 and base (row*3+9) in
// sb, copying base into a low register for the flag index. With an s32 idx
// helper parameter p and base swap (p in r8, base in r7; 73.6%); the s16
// parameter here gets the allocation right but adds a sign extension.

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

static inline void SetDigitVisible(struct ScoreDigits *p, s16 idx, s32 glyph)
{
    if (glyph == 0x3e)
        p->slots[idx].flags |= 1;
    else
        p->slots[idx].flags &= ~1;
}

// confidence=low: draws a three-digit number into a row of digit widgets, hiding leading zeros.
void sub_0810FDDC(struct ScoreDigits *p, s32 row, s32 n)
{
    u8 base = (row * 3) + 9;
    s32 glyph;
    glyph = 0x3e;
    while (n > 99) {
        glyph++;
        n -= 100;
    }

    SetDigitVisible(p, base, glyph);
    Anim_SelectWidgetGlyph(&p->slots[base], p->glyphs, glyph);
    glyph = 0x3e;
    {
        s32 tens = base + 1;
        s32 ones = base + 2;
        while (n > 9) {
            glyph++;
            n -= 10;
        }

        SetDigitVisible(p, tens, glyph);
        Anim_SelectWidgetGlyph(&p->slots[base + 1], p->glyphs, glyph);
        glyph = 0x3e;
        while (n > 0) {
            glyph++;
            n--;
        }

        Anim_SelectWidgetGlyph(&p->slots[ones], p->glyphs, glyph);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0810FDDC);
#endif
