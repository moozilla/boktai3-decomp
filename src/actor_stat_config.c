#include "actor_stat_config.h"
#include "global.h"

void ActorStats_SetChoices(struct ActorStatChoices *choices, s32 count, u32 *values)
{
    s32 i;
    if (count > 4)
        count = 4;
    for (i = 0; i < count; i++)
        choices->values[i] = values[i];
    choices->count = count;
}

s32 ModifierStats_ApplyKind7(struct ActorStats *, s32);

void ActorStats_ConfigureScaledParameters(struct ActorStats *stats, s32 parameter90,
                                          s32 parameter94, s32 parameter9C, s32 parameterA0,
                                          s32 parameterA4, s32 parameterA8)
{
    s32 scaled = stats->factor51A * 150;
    scaled = ModifierStats_ApplyKind7(stats, scaled);
    stats->limit88 = stats->factor336 * 10;
    stats->rate84 = stats->rate519 * 5;
    stats->parameter90 = parameter90;
    stats->parameter94 = parameter94;
    stats->scaledParameter98 = scaled;
    stats->parameter9C = parameter9C;
    stats->parameterA0 = parameterA0;
    stats->parameterA4 = parameterA4;
    stats->parameterA8 = parameterA8;
}
