#include "global.h"
#include "modifier_list.h"
#define StatModifier_ScaleAtOwnerThreshold sub_0811E814
s32 Div(s32, s32);
s32 sub_0811E814(struct StatModifier *modifier, s32 value)
{
    s32 result;
    struct StatModifierOwnerStats *stats;
    if (!modifier->enabled) return value;
    stats = &modifier->owner->stats;
    if (stats->current < stats->threshold) return value;
    result = Div(modifier->amount * value, 10);
    if (result < 0) result = 1;
    return result;
}
