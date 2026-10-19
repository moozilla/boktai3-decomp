#include "global.h"
#include "modifier_list.h"
#ifdef NONMATCHING
// fuzzy: 81.6%
// foundry draft (worker best 81.6%); what differs is mostly register allocation and instruction order
#define StatModifier_AddForOwnerMode sub_0811E7C8
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
s32 sub_0811E7C8(register struct StatModifier *modifier, s32 value)
{
    s32 result;
    if (modifier->enabled == 0)
        result = value;
    else {
        u32 mask = 0x800;
        if (!((gUnk_030053F4 | gUnk_030053F0) & mask) || modifier->owner->mode == 0)
            result = value;
        else {
            result = value + modifier->amount;
            if (result < 0)
                result = 1;
        }
    }
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811E7C8);
#endif
