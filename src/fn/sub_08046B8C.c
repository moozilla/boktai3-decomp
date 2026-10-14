#include "damage_number_display.h"
u32 DamageNumberDisplay_GetActiveBit(struct DamageNumberDisplay *, u32);
s32 DamageNumberDisplay_FindAvailableGroup(struct DamageNumberDisplay *display, s32 priority)
{
    struct DamageNumberGroup *group;
    s32 index = 0, result;
    do {
        if (!DamageNumberDisplay_GetActiveBit(display, index)) {
            result = index;
            goto done;
        }
        ++index;
    } while (index <= 3);
    group = display->groups;
    index = 0;
    do {
        if (group->priority <= priority) {
            result = index;
            goto done;
        }
        ++index;
        ++group;
    } while (index <= 3);
    result = -1;
done:
    return result;
}
