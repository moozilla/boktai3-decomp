#include "global.h"

struct P { u32 a, b; };
void sub_082151E4(u8 *, u32);
void sub_082144A4(u8 *, u8 *, u32);
void sub_08225B28(u8 *, u8 *);
void sub_08215284(u8 *, u32);
u32 sub_0821A520(u32, u32);
void sub_080641B0(u8 *);

void sub_08064460(u8 *p)
{
    u8 *a = p + 0x90;
    u8 *b;
    sub_082151E4(a, 0x9247);
    b = p + 0x64;
    sub_082144A4(b, a, 0);
    sub_08225B28(p + 0x1c, b);
    sub_08215284(a, 0x193);
    *(struct P *)(p + 0x80) = *(struct P *)(p + 0x28);
    *(u32 *)(p + 0x120) = sub_0821A520(0x922E, 0x1DF8);
    *(u16 *)(p + 0x138) = 7;
    sub_080641B0(p);
}
