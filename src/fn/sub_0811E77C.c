#include "global.h"
#include "modifier_list.h"
#include "engine.h"
extern u32 gUnk_030053F0;
s32 StatModifier_AddForGlobalMode(void *arg, s32 value)
{
    struct StatModifier *modifier = arg;
    s32 result;
    s32 input = value;
    if (modifier->enabled == 0)
        do {
            return input;
        } while (0);
    {
        u16 mask = 0x800;
        if (!((gEngineFlags | gUnk_030053F0) & mask) || modifier->owner->mode == 1)
            do {
                return input;
            } while (0);
    }
    result = input + modifier->amount;
    if (result < 0)
        result = 1;
    return result;
}
