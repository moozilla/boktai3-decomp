#include "display_animation.h"

s32 DisplayAnimation_ClearFlags(struct DisplayAnimation *p, u32 mask)
{
    u32 *q;
    if (p->kind == 0) {
        return -1;
    }
    q = p->displayFlags;
    *q &= ~mask;
    return 0;
}
