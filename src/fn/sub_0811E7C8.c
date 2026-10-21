#include "global.h"
#include "modifier_list.h"
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
s32 StatModifier_AddForOwnerMode(struct StatModifier *modifier_, s32 value_)
{
    struct StatModifier *modifier = modifier_;
    s32 value = value_;
    s32 result;
    if (modifier->enabled == 0)
        do {
            result = value;
        } while (0);
    else {
        u32 mask = 0x800;
        if (!((gUnk_030053F4 | gUnk_030053F0) & mask) || modifier->owner->mode == 0)
            do {
                result = value;
            } while (0);
        else {
            result = value + modifier->amount;
            if (result < 0)
                result = 1;
        }
    }
    return result;
}
