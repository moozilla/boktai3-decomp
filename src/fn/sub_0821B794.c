#include "global.h"
u32 sub_0821B738(void);
u32 sub_0821AF2C(u32, u32, u32);
u32 sub_0821B794(void)
{
    u32 r = sub_0821B738();
    if (r != 0)
        return sub_0821AF2C(r, 0, 0);
    return 0;
}
