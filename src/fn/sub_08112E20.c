#include "global.h"
void sub_08214514(u8 *);
void sub_08219D38(u8 *);
void sub_08112E20(u8 **pp)
{
    u8 *p = pp[0];
    if (p[0x20] != 0) {
        sub_08214514(p + 0x1c);
        sub_08219D38(pp[0]);
        ((u8 *)pp)[0x19] = 0;
    }
}
