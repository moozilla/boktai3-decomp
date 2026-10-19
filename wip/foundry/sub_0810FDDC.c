#define LinkMenu_DrawThreeDigitNumber sub_0810FDDC
#include "global.h"
struct LinkMenuDigitSprite {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x54];
};
struct LinkMenuDigits {
    u8 unknown0[0x24];
    struct LinkMenuDigitSprite digits[18];
    u8 unknown6E4[0x20];
    u8 graphics[1];
};
s32 sub_08219728(void *, void *, u32);
void sub_0810FDDC(struct LinkMenuDigits *menu, s32 row, s32 value)
{
    s32 digit = row * 3 + 9;
    s32 glyph = 0x3E;
    s32 next;
    s32 last;
    while (value > 99) {
        glyph++;
        value -= 100;
    }
    if (glyph == 0x3E)
        menu->digits[digit].flags |= 1;
    else
        menu->digits[digit].flags &= ~1;
    sub_08219728(&menu->digits[digit], menu->graphics, (u16)glyph);
    glyph = 0x3E;
    next = digit + 1;
    last = digit + 2;
    while (value > 9) {
        glyph++;
        value -= 10;
    }
    if (glyph == 0x3E)
        menu->digits[next].flags |= 1;
    else
        menu->digits[next].flags &= ~1;
    sub_08219728(&menu->digits[digit + 1], menu->graphics, (u16)glyph);
    glyph = 0x3E;
    while (value > 0) {
        glyph++;
        value--;
    }
    sub_08219728(&menu->digits[last], menu->graphics, (u16)glyph);
}
