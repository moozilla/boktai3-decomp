#include "global.h"

struct Blk { u32 w[7]; };

struct Obj {
    u32 f0;
    u8 pad4;
    u8 f5, f6, f7, f8, f9;
    u8 padA[2];
    u32 fC;
    u32 f10, f14;
    u8 pad18[4];
    u32 f1C, f20;
};

void sub_0821452C(struct Obj *a, struct Obj *b, struct Blk *c, struct Blk *d)
{
    u8 t;
    *c = *d;
    a->f0 = b->f0;
    t = b->f5;
    a->f6 = b->f6;
    a->f7 = b->f7;
    a->f8 = b->f8;
    a->f9 = b->f9;
    a->f5 = t;
    a->fC = (u32)c;
    {
        u32 x = b->f10, y = b->f14;
        a->f10 = x;
        a->f14 = y;
    }
    {
        u32 x = b->f20;
        u32 y = b->f1C;
        a->f1C = y;
        a->f20 = x;
    }
}
