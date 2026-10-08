#include "global.h"
void sub_0824923C(u8 *, u32);
void sub_08063B24(u8 *);
void sub_08225B74(u8 *);
u32 sub_08063C60(u8 *p)
{
    sub_0824923C(p, *(u32 *)(p + 0x140));
    sub_08063B24(p);
    sub_08225B74(p + 0x1c);
    return 0;
}
