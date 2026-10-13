#include "global.h"
void ActorShadow_Disable(void *);
void sub_08013728(void *);
u32 sub_0802D88C(u8 *p)
{
    u8 v = p[0x218];
    if (v == 1)
        ActorShadow_Disable(p + 0x244);
    else if (v == 2)
        sub_08013728(p + 0x244);
    return 0;
}
