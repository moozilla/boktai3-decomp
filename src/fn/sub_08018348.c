#include "global.h"
extern u8 *gUnk_02000090;
int sub_080174B8(void *, int);
#define Weather_Start sub_08018348
int Weather_Start(void)
{
    u8 *p = gUnk_02000090;
    if (!p)
        return -1;
    sub_080174B8(p, 1);
    p[32] = 1;
}
