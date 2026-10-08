#include "global.h"
void sub_081FEC38(void *, s32);
u32 sub_081FEC80(void *p)
{
    s32 i = 0;
    do {
        sub_081FEC38(p, i);
        i++;
    } while (i <= 3);
    return 0;
}
