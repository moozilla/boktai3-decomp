#include "global.h"
struct A { u16 v[20]; };
void sub_080154EC(u32, u32, struct A *);
void sub_08015B84(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    struct A s;
    s.v[0] = b;
    s.v[1] = c;
    s.v[3] = d;
    s.v[4] = 0xc;
    s.v[5] = 0xc;
    s.v[6] = 4;
    s.v[7] = 4;
    s.v[8] = 0x3c;
    s.v[9] = 8;
    s.v[10] = 0;
    s.v[11] = 0xff;
    s.v[12] = 0xff;
    s.v[13] = 0;
    s.v[14] = 0xff;
    s.v[15] = 0;
    s.v[16] = 0xff;
    s.v[17] = 0;
    s.v[18] = 0xff;
    s.v[19] = 0;
    sub_080154EC(a, e, &s);
}
