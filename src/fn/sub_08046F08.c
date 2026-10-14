#include "damage_number_display.h"
void DamageNumberDisplay_ClearActiveBit(struct DamageNumberDisplay *, u32);
void DamageNumberDisplay_RemoveGroup(struct DamageNumberDisplay *display, s32 index)
{
    struct DamageNumberGroup *group = &display->groups[index];
    struct DamageNumberGlyph *digit;
    s32 i = 0;
    if (i < group->digitCount) {
        digit = group->digits;
        do {
            Effect_Remove(digit);
            ++digit;
            ++i;
        } while (i < group->digitCount);
    }
    group->digitCount = 0;
    DamageNumberDisplay_ClearActiveBit(display, index);
}
