#include "global.h"
extern u16 gUnk_03004BD8;
void sub_081E2794(void)
{
    u16 *r = &gUnk_03004BD8;
    u32 m = ~3;
    *r = m & *r;
}
