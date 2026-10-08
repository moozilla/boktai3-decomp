#include "global.h"
void sub_0811D024(s32 *s, s32 d, s16 *o)
{
    o[0] = s[0] >> 12;
    o[1] = (s[1] + d) >> 12;
    o[2] = s[2] >> 12;
}
