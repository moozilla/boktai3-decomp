#include "global.h"
#include "modifier_list.h"
#ifdef NONMATCHING
// fuzzy: 81.6%
// foundry draft (worker best 81.6%); what differs is mostly register allocation and instruction order
#define StatModifier_AddForGlobalMode sub_0811E77C
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
s32 sub_0811E77C(void *arg, s32 value)
{
    struct StatModifier *modifier = arg;
    s32 result;
    s32 input = value;
    if (modifier->enabled == 0) return input;
    {
        u16 mask = 0x800;
        if (!((gUnk_030053F4 | gUnk_030053F0) & mask) || modifier->owner->mode == 1)
            return input;
    }
    result = input + modifier->amount;
    if (result < 0) result = 1;
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811E77C);
#endif
