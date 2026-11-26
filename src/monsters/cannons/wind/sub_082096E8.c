#include "global.h"
struct S {
    u8 a[0x120];
    s16 h;
};
u32 FanCannon_UpdateInactive(struct S *p)
{
    if (p->h > 0)
        p->h--;
    return 0;
}
