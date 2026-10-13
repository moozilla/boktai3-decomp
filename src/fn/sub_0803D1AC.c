#include "global.h"

void sub_0811CD08(u8 *);
void sub_0811B394(u8 *);
void sub_08112C08(s32);

void sub_0803D1AC(u32 a, u8 *p)
{
    sub_0811CD08(p + 0x600);
    *(s32 *)(p + 0x854) = -1;
    sub_0811B394(p + 0x860);
    sub_08112C08(*(s8 *)(p + 0x850));
}
