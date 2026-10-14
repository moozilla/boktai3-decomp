#include "damage_number_display.h"
void sub_08217EEC(void *, void *, u32);
void sub_08217E40(void *, u32);
extern const u32 gUnk_086059C0[];
#ifdef NONMATCHING
// fuzzy: 63.1%
// Decimal setup is recovered; register allocation and projection-loop lifetimes still differ.
u32 DamageNumberDisplay_RenderDigits(struct DamageNumberDisplay *display,
                                     struct DamageNumberGroup *group, u32 amount, u32 glyphBase,
                                     struct ContactPosition *initialScreen)
{
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index, value;
    u32 horizontalOffset;
    s32 count;
    index = 0;
    if (index < group->digitCount) {
        digit = group->digits;
        do {
            Effect_Remove(digit);
            ++digit;
            ++index;
        } while (index < group->digitCount);
    }
    group->digitCount = 0;
    group->anchorY = initialScreen->y;
    group->raisedY = initialScreen->y + 6;
    while (amount > 9999)
        amount -= 10000;
    count = 0;
    while (amount > 999) {
        ++count;
        amount -= 1000;
    }
    sub_08217EEC(&group->digits[3], display->resource, glyphBase + count);
    if (count > 0)
        group->digitCount = 4;
    count = 0;
    while (amount > 99) {
        ++count;
        amount -= 100;
    }
    sub_08217EEC(&group->digits[2], display->resource, glyphBase + count);
    if (!group->digitCount && count > 0)
        group->digitCount = 3;
    count = 0;
    while (amount > 9) {
        ++count;
        amount -= 10;
    }
    sub_08217EEC(&group->digits[1], display->resource, glyphBase + count);
    if (!group->digitCount && count > 0)
        group->digitCount = 2;
    count = 0;
    while (amount != 0) {
        ++count;
        --amount;
    }
    sub_08217EEC(&group->digits[0], display->resource, glyphBase + count);
    if (!group->digitCount)
        group->digitCount = 1;
    digit = group->digits;
    index = 0;
    if (index < group->digitCount) {
        horizontalOffset = 0;
        do {
            s32 angle = (group->phaseFrame << 4) + (index << 5);
            if (angle > 128)
                angle = 128;
            DamageNumber_ProjectPosition(&screen, &group->position);
            digit->x = (u16)screen.x - horizontalOffset - gUnk_086059C0[group->digitCount];
            value = gUnk_086149C4[angle & 255] * 16;
            if (value >= 0)
                value >>= 12;
            else
                value = -((-value) >> 12);
            digit->y = (u16)screen.y - value;
            sub_08217E40(digit, 0);
            horizontalOffset += 5;
            ++index;
            ++digit;
        } while (index < group->digitCount);
    }
    /* The original leaves r0 unspecified; both callers discard the result. */
}
#else
INCLUDE_ASM("asm/nonmatching", DamageNumberDisplay_RenderDigits);
#endif
