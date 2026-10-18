#include "global.h"
#include "actor_stats.h"
s32 ModifierStats_ApplyKind7(struct ActorStats *, s32);
void ActorStats_ConfigureScaledParameters(struct ActorStats *p, s32 a, s32 b, s32 c, s32 d, s32 e,
                                          s32 f)
{
    s32 scaled = p->factor51A * 150;
    scaled = ModifierStats_ApplyKind7(p, scaled);
    p->limit88 = p->factor336 * 10;
    p->rate84 = p->rate519 * 5;
    p->parameter90 = a;
    p->parameter94 = b;
    p->scaledParameter98 = scaled;
    p->parameter9C = c;
    p->parameterA0 = d;
    p->parameterA4 = e;
    p->parameterA8 = f;
}
