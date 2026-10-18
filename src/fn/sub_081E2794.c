#include "global.h"
extern u16 gUnk_03004BD8;
void TimedPresentation_Destroy(void)
{
    u16 *r = &gUnk_03004BD8;
    u32 m = ~3;
    *r = m & *r;
}
