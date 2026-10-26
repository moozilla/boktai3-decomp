#include "global.h"
#include "render.h"
void TimedPresentation_Destroy(void)
{
    u16 *r = &gDispCntShadow;
    u32 m = ~3;
    *r = m & *r;
}
