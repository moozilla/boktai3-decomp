#include "damage_number_display.h"
#define DamageNumberDisplay_FindSource   sub_08046B58
#define DamageNumberDisplay_GetActiveBit sub_08046B4C
u32 DamageNumberDisplay_GetActiveBit(struct DamageNumberDisplay *, u32);
s32 DamageNumberDisplay_FindSource(struct DamageNumberDisplay *display, u32 key)
{
    struct DamageNumberGroup *group = display->groups;
    s32 index = 0, result;
    do {
        if (DamageNumberDisplay_GetActiveBit(display, index) && group->sourceKey == key) {
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
