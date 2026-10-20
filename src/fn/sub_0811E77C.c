#include "global.h"
#include "modifier_list.h"
// MATCHING_CONCESSION: register-pinning
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
s32 StatModifier_AddForGlobalMode(void *arg, s32 value)
{
    register struct StatModifier *modifier asm("r4") = arg;
    s32 result;
    register s32 input asm("r3") = value;
    if (modifier->enabled == 0)
        return input;
    {
        u16 mask = 0x800;
        if (!((gUnk_030053F4 | gUnk_030053F0) & mask) || modifier->owner->mode == 1)
            return input;
    }
    result = input + modifier->amount;
    if (result < 0)
        result = 1;
    return result;
}
