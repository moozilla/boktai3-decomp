#include "global.h"
s32 sub_0811BB58(void);
s32 sub_0811BB1C(void);
void sub_0811D600(s32, s32);
s32 sub_0811D5A8(s32, s32);
s32 sub_081123C0(s32 p)
{
    s32 a = sub_0811BB58();
    s32 b = sub_0811BB1C();
    s32 r;
    sub_0811D600(b, p);
    r = sub_0811D5A8(b, a);
    if (r > 0)
        return 1;
    return -1;
}
