#include "global.h"
s32 sub_0822CE44(void);
s32 sub_0822CF6C(s32);
s32 sub_0822CF44(s32);
s32 sub_0822CF94(s32 a)
{
    s32 r;
    if (sub_0822CE44())
        r = sub_0822CF6C(a);
    else
        r = sub_0822CF44(a);
    return r;
}
