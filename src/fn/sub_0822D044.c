#include "global.h"
s32 sub_0822CE44(void);
s32 sub_0822D020(s32);
s32 sub_0822CFFC(s32);
s32 sub_0822D044(s32 a)
{
    s32 r;
    if (sub_0822CE44())
        r = sub_0822D020(a);
    else
        r = sub_0822CFFC(a);
    return r;
}
