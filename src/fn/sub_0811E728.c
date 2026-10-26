#include "global.h"
#include "modifier_list.h"
#include "engine.h"
#include "ram.h"
s32 Div(s32, s32);
s32 StatModifier_ScaleForGlobalMode(struct StatModifier *modifier_, s32 value)
{
    struct StatModifier *modifier = modifier_;
    s32 result;
    s32 input = value;
    if (modifier->enabled == 0)
        do {
            return input;
        } while (0);
    {
        u16 mask = 0x800;
        if (!((gEngineFlags | gScenePauseFlags) & mask) || modifier->owner->mode == 1)
            do {
                return input;
            } while (0);
    }
    result = Div(modifier->amount * input, 10);
    if (result < 0)
        result = 1;
    return result;
}
