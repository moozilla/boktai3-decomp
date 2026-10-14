#include "display_animation.h"
#define DisplayAnimation_TestFlags sub_0804271C

u32 DisplayAnimation_TestFlags(struct DisplayAnimation *p, u32 mask)
{
    if (p->kind == 0)
        return 0;
    return *p->displayFlags & mask;
}
