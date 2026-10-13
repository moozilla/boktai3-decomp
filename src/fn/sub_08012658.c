#include "animated_hazard.h"

void sub_08012620(void *, struct AnimatedHazard *);

u32 sub_08012658(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e = s->hazards;
    s32 i;
    for (i = 7; i >= 0; i--, e++) {
        sub_08012620(s, e);
    }
    return gUnk_02000050 = 0;
}
