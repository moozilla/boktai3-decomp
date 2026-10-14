#include "damage_number_display.h"
#define DamageNumberDisplay_FindAvailableGroup sub_08046B8C
#define DamageNumberDisplay_GetActiveBit       sub_08046B4C
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
