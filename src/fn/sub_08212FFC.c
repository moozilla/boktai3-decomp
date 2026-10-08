#include "global.h"
s32 sub_08212FC4(void);
u32 sub_08212FFC(void)
{
    s32 v = sub_08212FC4();
    u32 r = 0;
    if (v > 9)
        r = 1;
    return r;
}
