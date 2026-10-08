#include "global.h"

struct Obj {
    u32 f0;
    u8 pad4;
    u8 f5, f6, f7, f8, f9;
    u8 padA[0xE];
    u32 f18;
};

void sub_0821456C(struct Obj *o)
{
    o->f0 = 0;
    o->f5 = 1;
    o->f18 = 0;
    o->f8 = 0x40;
    o->f9 = 0x40;
    o->f6 = 0;
    o->f7 = 2;
}
