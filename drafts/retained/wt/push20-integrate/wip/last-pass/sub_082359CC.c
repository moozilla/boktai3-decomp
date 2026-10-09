#include "global.h"
struct Blob59 { u32 words[8]; };
struct Blob59 *sub_0821A520(u32, u32);
void sub_082196C4(struct Blob59 *, struct Blob59 *);
void sub_0821983C(u8 *, struct Blob59 *, u32, u32, u32, u32, u32, void *);
void sub_082359CC(u8 *p)
{
    struct Blob59 *data = sub_0821A520(0xcb05, 0x5d04);
    struct Blob59 copy = *data;
    u32 kind, zero, x, value;
    u16 *dest;
    sub_082196C4(&copy, data);
    value = *(u16 *)(p + 0x34c);
    kind = 48;
    if (value == 0) kind = 47;
    x = *(u16 *)(p + 0x84) - 8;
    dest = (u16 *)(p + 0x7c);
    zero = 0;
    *dest = x;
    *(u16 *)(p + 0x7e) = *(u16 *)(p + 0x86) - 40;
    *(u16 *)(p + 0x80) = *(u16 *)(p + 0x88);
    sub_0821983C(p + 0x1c, &copy, kind + 104, 50, 1, zero, 60, p + 0x84);
    p[0x51] = 34;
    p[0x50] = 34;
}
