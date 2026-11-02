#include "damage_number_display.h"
void DamageNumberDisplay_RemoveGroup(struct DamageNumberDisplay *, s32);
extern const u32 gUnk_086059C0[];
extern const s16 gUnk_08614A84[];
s32 DamageNumberDisplay_UpdateLeaving(struct DamageNumberDisplay *display,
                                      struct DamageNumberGroup *group, s32 groupIndex)
{
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index;
    s32 value;
    s32 scaledSin;
    s32 count;
    DamageNumber_ProjectPosition(&screen, &group->position);
    digit = group->digits;
    index = 0;
    count = group->digitCount;
    if (index < count) {
        struct ContactPosition *projected = &screen;
        const u32 *centers;
        const s16 *sine;
        centers = gUnk_086059C0;
        sine = gUnk_08614A84;
        do {
            u32 left = (u16)projected->x - index * 5;
            digit->x = left - centers[count];
            value = *sine * 16;
            if (value >= 0)
                scaledSin = value >> 12;
            else
                scaledSin = -((-value) >> 12);
            digit->y = (u16)projected->y - scaledSin;
            ++index;
            ++digit;
            count = group->digitCount;
        } while (index < count);
    }
    if ((u8)++group->phaseFrame > 9)
        DamageNumberDisplay_RemoveGroup(display, groupIndex);
    return 0;
}
