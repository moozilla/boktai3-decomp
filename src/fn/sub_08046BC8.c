#include "damage_number_display.h"
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void sub_08217E40(void *, u32);
extern const u32 gUnk_086059C0[];
inline static void DamageNumber_ProjectRelativePosition(struct ContactPosition *screen,
                                                        const struct ContactPosition *world,
                                                        const u16 *camera)
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
    screen->x = (((u16)screen->x) - camera[0]) + 120;
    screen->y = (y - camera[1]) + 80;
    screen->z = depth - camera[2];
}

u32 DamageNumberDisplay_RenderDigits(struct DamageNumberDisplay *display,
                                     struct DamageNumberGroup *group, u32 amount, u32 glyphBase,
                                     struct ContactPosition *initialScreen)
{
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index;
    s32 value;
    s32 scaledSin;
    const u16 *camera;
    s32 count;
    for (index = 0; index < group->digitCount; index++)
        Effect_Remove(&group->digits[index]);
    group->digitCount = 0;
    group->anchorY = initialScreen->y;
    group->raisedY = initialScreen->y + 6;
    if (amount > 9999) {
        do {
            amount -= 10000;
        } while (amount > 9999);
    }

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
        do {
            s32 angle = (group->phaseFrame << 4) + (index << 5);
            camera = gCameraOrigin;
            if (angle > 128)
                angle = 128;
            DamageNumber_ProjectRelativePosition(&screen, &group->position, camera);
            digit->x = (u16)screen.x - index * 5 - gUnk_086059C0[group->digitCount];
            value = gUnk_086149C4[angle & 255] * 16;
            if (value >= 0)
                scaledSin = value >> 12;
            else
                scaledSin = -((-value) >> 12);
            digit->y = ((u16)screen.y) - scaledSin;
            sub_08217E40(digit, 0);
            ++index;
            ++digit;
        } while (index < group->digitCount);
    }
}
