#include "global.h"
#include "modifier_list.h"
#ifdef NONMATCHING
// fuzzy: 82.9%
// foundry draft (worker best 82.9%); what differs is mostly register allocation and instruction order
#define StatModifier_ScaleForGlobalMode sub_0811E728
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
s32 Div(s32, s32);
s32 sub_0811E728(struct StatModifier *modifier, s32 value)
{
    s32 result;
    s32 input = value;
    if (modifier->enabled == 0)
        return input;
    {
        u16 mask = 0x800;
        if (!((gUnk_030053F4 | gUnk_030053F0) & mask) || modifier->owner->mode == 1)
            return input;
    }
    result = Div(modifier->amount * input, 10);
    if (result < 0)
        result = 1;
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811E728);
#endif
