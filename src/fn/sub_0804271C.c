#include "display_animation.h"
/* DisplayAnimation_TestFlags: Returns masked display flags or zero for an inactive wrapper; pairs with SetFlags and ClearFlags */
#define DisplayAnimation_TestFlags sub_0804271C
#define DisplayAnimation_TestFlags sub_0804271C

u32 DisplayAnimation_TestFlags(struct DisplayAnimation *p, u32 mask)
{
    if (p->kind == 0)
        return 0;
    return *p->displayFlags & mask;
}
