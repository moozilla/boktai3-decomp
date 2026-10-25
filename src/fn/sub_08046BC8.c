#include "damage_number_display.h"
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void sub_08217E40(void *, u32);
extern const u32 gUnk_086059C0[];
#ifdef NONMATCHING
// fuzzy: 78.3%
// fix.py best (sweep after the inlined-insn fix)
// Decimal setup is recovered; register allocation and projection-loop lifetimes still differ.
inline static void DamageNumber_ProjectPosition_(struct ContactPosition *screen,
                                                 const struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 vertical;
    s32 y;
    s32 depth;
    s32 scaledSum;
    screen->x = DamageNumber_ProjectScale(3 * (x - z));
    scaledSum = DamageNumber_ProjectScale(3 * (x + z));
    sum = scaledSum;
    vertical = DamageNumber_ProjectScale(3 * world->y);
    y = sum - vertical;
    depth = sum + vertical;
    screen->x = (((u16)screen->x) - (*((typeof(gUnk_03005418) *)0x03005418))[0]) + 120;
    screen->y = (y - (*((typeof(gUnk_03005418) *)0x03005418))[1]) + 80;
    screen->z = depth - (*((typeof(gUnk_03005418) *)0x03005418))[2];
}

u32 DamageNumberDisplay_RenderDigits(struct DamageNumberDisplay *display,
                                     struct DamageNumberGroup *group, u32 amount, u32 glyphBase,
                                     struct ContactPosition *initialScreen)
{
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index;
    s32 value;
    u32 horizontalOffset;
    s32 count;
    typeof(digit) t1;
    index = 0;
    if (index < group->digitCount) {
        digit = (t1 = group->digits);
        do {
            Effect_Remove(t1);
            ++t1;
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

    SpriteDrawable_SelectTileFrame(&group->digits[3], display->resource, glyphBase + count);
    if (count > 0)
        group->digitCount = 4;
    count = 0;
    while (amount > 99) {
        ++count;
        amount -= 100;
    }

    SpriteDrawable_SelectTileFrame(&group->digits[2], display->resource, glyphBase + count);
    if ((!group->digitCount) && (count > 0))
        group->digitCount = 3;
    count = 0;
    while (amount > 9) {
        ++count;
        amount -= 10;
    }

    SpriteDrawable_SelectTileFrame(&group->digits[1], display->resource, glyphBase + count);
    if ((!group->digitCount) && (count > 0))
        group->digitCount = 2;
    count = 0;
    while (amount != 0) {
        ++count;
        --amount;
    }

    SpriteDrawable_SelectTileFrame(&group->digits[0], display->resource, glyphBase + count);
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
            DamageNumber_ProjectPosition_(&screen, &group->position);
            digit->x = (((u16)screen.x) - horizontalOffset) - gUnk_086059C0[group->digitCount];
            value = gUnk_086149C4[angle & 255] * 16;
            if (value >= 0)
                value >>= 12;
            else
                value = -((-value) >> 12);
            digit->y = ((u16)screen.y) - value;
            sub_08217E40(digit, 0);
            horizontalOffset += 5;
            ++index;
            ++digit;
        } while (index < group->digitCount);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", DamageNumberDisplay_RenderDigits);
#endif
