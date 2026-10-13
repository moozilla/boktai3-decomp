#include "animated_hazard.h"

void AnimatedHazard_Initialize(struct AnimatedHazardManager *, struct AnimatedHazard *);

s32 sub_08012684(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e;
    s32 i;
    gUnk_02000050 = s;
    s->script = ScriptProc_Start(0x922E, 0xE92A);
    if (AnimData_Load(s->animData, 0xCD6C) == 0)
        return -1;
    e = s->hazards;
    for (i = 7; i >= 0; i--, e++) {
        AnimatedHazard_Initialize(s, e);
    }
    return 0;
}
