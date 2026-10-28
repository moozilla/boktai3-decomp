#include "global.h"
#include "render.h"


void AnimData_SetResourceBank(struct AnimResourceHeader *h)
{
    gAnimationResource = h;
    gAnimResourceSection14 = (u32)h + h->offset14;
    gAnimResourceSection0C = (u32)h + h->offsetC;
}
