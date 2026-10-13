#include "animated_hazard.h"

struct AnimatedHazardManager *sub_08012040(void);
void *sub_080126DC(void);
void sub_0801204C(void *, struct AnimatedHazard *, u32);

s32 sub_08012A88(void)
{
    struct AnimatedHazardManager *s = sub_08012040();
    struct AnimatedHazard *e;
    s32 i;
    if (s == 0) {
        s = sub_080126DC();
        if (s == 0)
            return -1;
    }
    e = s->hazards;
    for (i = 7; i >= 0; i--) {
        if (e->active != 0 && e->state <= 2)
            sub_0801204C(s, e, 3);
        e++;
    }
    return 0;
}
