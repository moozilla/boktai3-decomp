#include "animated_hazard.h"

struct AnimatedHazard *sub_08012728(struct AnimatedHazardManager *p)
{
    struct AnimatedHazard *q = p->hazards;
    s32 i = 0;
    while (i <= 7) {
        if (q->active == 0) {
            return q;
        }
        i++;
        q++;
    }
    return 0;
}
