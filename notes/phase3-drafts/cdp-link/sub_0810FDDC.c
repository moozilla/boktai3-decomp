#include "ui/widgets/scene_input.h"
// MATCHING_CONCESSION: register-pinning

struct ScoreDigits {
    u8 taskAndResources[0x24];
    struct AnimSprite slots[18];
    u8 unknown6E4[0x20];
    struct AnimSpriteResource glyphs;
};

static inline void SetDigitVisible(struct ScoreDigits *p, s32 idx, s32 glyph)
{
    if (glyph == 0x3e)
        p->slots[idx].flags |= 1;
    else
        p->slots[idx].flags &= ~1;
}

// Slots 9 through 17 hold the three rows of digits positioned by sub_0810FADC.
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
    SetDigitVisible(p, index, glyph);
    AnimSprite_SelectGlyph((u8 *)p->slots +
                               ((index << 1) + base) * (sizeof(struct AnimSprite) / 3),
                           &p->glyphs, (u16)glyph);
    glyph = 0x3e;
    {
        register s32 tens = base + 1;
        s32 ones;
        while (n > 9) {
            glyph++;
            n -= 10;
        }

        SetDigitVisible(p, tens, glyph);
        AnimSprite_SelectGlyph((u8 *)&p->slots[1] +
                                   ((index << 1) + base) * (sizeof(struct AnimSprite) / 3),
                               &p->glyphs, (u16)glyph);
        glyph = 0x3e;
        while (n > 0) {
            glyph++;
            n--;
        }

        ones = base + 2;
        AnimSprite_SelectGlyph(&p->slots[ones], &p->glyphs, (u16)glyph);
    }
}
