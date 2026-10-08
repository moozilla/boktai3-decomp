#include "global.h"
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);
void sub_0812B77C(u8 *p)
{
    u8 *a = p + 0xf4;
    u8 *b = p + 0x1c;
    sub_0821FF24(a, b, 0);
    sub_0821FF24(p + 0x148, b, 0);
    sub_0821FE40(a);
}
