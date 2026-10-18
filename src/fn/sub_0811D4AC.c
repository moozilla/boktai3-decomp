#define ActorStats_ConfigureScaledParameters sub_0811D4AC
#define ModifierStats_ApplyKind7             sub_0811E970
#include "global.h"
#include "actor_stats.h"
s32 sub_0811E970(struct ActorStats *, s32);
void sub_0811D4AC(struct ActorStats *p, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    s32 scaled = p->factor51A * 150;
    scaled = sub_0811E970(p, scaled);
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
