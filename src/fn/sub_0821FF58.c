#include "global.h"
struct O { u8 p[0x34]; u32 f34; u32 f38; u16 f3c; u16 pa; u16 f40; u16 pb; u16 f44; };
void sub_0821FF58(struct O *o, u16 a, u16 b, u32 c, u32 d, u16 e)
{
    o->f3c = a;
    o->f40 = b;
    o->f34 = c;
    o->f38 = d;
    o->f44 = e;
}
