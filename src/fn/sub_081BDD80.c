#include "global.h"
void sub_081BDD74(u8 *, u32);
u8 sub_0805E5D0(u8 *);
void sub_081BDD80(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    u32 z = 0;
    p[0] = a;
    sub_081BDD74(p, d);
    p[1] = b;
    p[2] = c;
    p[3] = sub_0805E5D0(p);
    p[8] = z;
}
