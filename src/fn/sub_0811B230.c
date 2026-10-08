#include "global.h"
void sub_0811D024(u32, u32, s16 *);
void sub_0811B164(u32, s32, s32, s32);
void sub_0811B230(u32 s, u32 a, u32 b)
{
    s16 buf[3];
    s16 *v = buf;
    sub_0811D024(a, b, v);
    sub_0811B164(s, buf[0], v[1], v[2]);
}
