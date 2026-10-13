#include "animated_hazard.h"

void sub_080125FC(void *, struct AnimatedHazard *);

s32 sub_08012630(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e = s->hazards;
    s32 i;
    for (i = 7; i >= 0; i--, e++) {
        if (e->active != 0)
            sub_080125FC(s, e);
    }
    return 0;
}
