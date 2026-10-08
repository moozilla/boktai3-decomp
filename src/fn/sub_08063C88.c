#include "global.h"

void sub_08214514(u8 *);
void sub_08225938(u8 *);
void sub_0815F738(u8 *);

s32 sub_08063C88(u8 *p)
{
    sub_08214514(p + 0x64);
    sub_08225938(p + 0x1c);
    sub_0815F738(p + 0x124);
    return 0;
}
