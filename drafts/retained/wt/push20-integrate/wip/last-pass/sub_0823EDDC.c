#include "global.h"
struct PairED { u32 a, b; };
u32 sub_0823D7D4(u8 *);
void sub_08215284(u8 *, u32);
void sub_0821FF58(u8 *, u32, u32, u32, u32, u32);
void sub_0823EDDC(u8 *p)
{
    u32 resource, zero;
    u8 *angle;
    *(struct PairED *)(p + 0xbe8) = *(struct PairED *)(p + 0x30);
    {
        u32 y = *(u16 *)(p + 0xbea) + 230;
        zero = 0;
        *(u16 *)(p + 0xbea) = y;
    }
    {
        u32 direction = ((p[0x3a4] + 5) & 7) << 5;
        angle = p + 0xbfc;
        *angle = direction;
    }
    resource = sub_0823D7D4(p);
    sub_08215284(p + 0xb70, 76);
    sub_0821FF58(p + 0xb8c, resource, *(u16 *)(p + 0xbfc), 0x2000, zero, *(u16 *)(p + 0xbfe));
    p[0xbce] = *angle;
    *(u16 *)(p + 0xc0a) = zero;
}
