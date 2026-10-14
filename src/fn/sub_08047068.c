// MATCHING_CONCESSION: register-pinning
#include "damage_number_display.h"
void DamageNumberDisplay_RemoveGroup(struct DamageNumberDisplay *, s32);
extern const u32 gUnk_086059C0[];
extern const s16 gUnk_08614A84[];
s32 DamageNumberDisplay_UpdateLeaving(struct DamageNumberDisplay *display,
                                      struct DamageNumberGroup *slot, s32 slotIndex)
{
    register s32 groupIndex asm("r8");
    register struct DamageNumberGroup *group asm("r12");
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index;
    register s32 value asm("r0");
    s32 scaledSin;
    u32 x;
    s32 count;
    group = slot;
    groupIndex = slotIndex;
    {
        register struct ContactPosition *projected asm("r5") = &screen;
        register struct ContactPosition *world asm("r6") = &group->position;
        struct DamageNumberGroup *lowGroup;
        s32 x;
        s32 z;
        s32 scaledSum;
        s32 sum, vertical, y, depth;
        register s32 pair asm("r1");
        lowGroup = group;
        x = lowGroup->position.x;
        z = world->z;
        projected->x = DamageNumber_ProjectScale(3 * (x - z));
        pair = x + z;
        scaledSum = DamageNumber_ProjectScale(3 * pair);
        sum = scaledSum;
        vertical = DamageNumber_ProjectScale(3 * world->y);
        y = sum - vertical;
        depth = sum + vertical;
        projected->x = (u16)projected->x - gUnk_03005418[0] + 120;
        projected->y = y - gUnk_03005418[1] + 80;
        projected->z = depth - gUnk_03005418[2];
    }
    digit = group->digits;
    index = 0;
    {
        register struct DamageNumberGroup *current asm("r1") = group;
        count = current->digitCount;
    }
    if (index < count) {
        struct ContactPosition *projected = &screen;
        const u32 *centers;
        const s16 *sine;
        asm("" : "+r"(projected));
        centers = gUnk_086059C0;
        sine = gUnk_08614A84;
        x = (u16)projected->x;
        do {
            digit->x = x - centers[count];
            value = *sine * 16;
            if (value >= 0)
                scaledSin = value >> 12;
            else
                scaledSin = -((-value) >> 12);
            digit->y = (u16)projected->y - scaledSin;
            x -= 5;
            ++index;
            ++digit;
            {
                register struct DamageNumberGroup *current asm("r1") = group;
                count = current->digitCount;
            }
        } while (index < count);
    }
    {
        struct DamageNumberGroup *current = group;
        u32 frame;
        frame = current->phaseFrame + 1;
        current->phaseFrame = frame;
        if ((u8)frame > 9)
            DamageNumberDisplay_RemoveGroup(display, groupIndex);
    }
    return 0;
}
