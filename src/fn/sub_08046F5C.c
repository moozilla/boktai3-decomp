// MATCHING_CONCESSION: register-pinning
#include "damage_number_display.h"
void DamageNumberGroup_AdvancePhase(struct DamageNumberGroup *);
extern const u32 gUnk_086059C0[];
s32 DamageNumberDisplay_UpdateBouncing(struct DamageNumberDisplay *display,
                                       struct DamageNumberGroup *group, u32 groupIndex)
{
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index;
    u32 horizontalOffset;
    register s32 value asm("r0");
    s32 scaledSin;
    DamageNumber_ProjectPosition(&screen, &group->position);
    digit = group->digits;
    index = 0;
    if (index < group->digitCount) {
        struct ContactPosition *projected = &screen;
        const u32 *centers;
        const s16 *sine;
        centers = gUnk_086059C0;
        sine = gUnk_086149C4;
        horizontalOffset = 0;
        do {
            s32 angle = (group->phaseFrame << 4) + (index << 5);
            if (angle > 96)
                angle = 96;
            digit->x = (u16)projected->x - horizontalOffset - centers[group->digitCount];
            value = sine[angle & 255] * 16;
            if (value >= 0)
                scaledSin = value >> 12;
            else
                scaledSin = -((-value) >> 12);
            digit->y = (u16)projected->y - scaledSin;
            horizontalOffset += 5;
            ++index;
            ++digit;
        } while (index < group->digitCount);
    }
    ++group->phaseFrame;
    if (group->phaseFrame > 19)
        DamageNumberGroup_AdvancePhase(group);
    return 0;
}
