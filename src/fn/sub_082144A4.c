#include "global.h"

struct Obj {
    u32 f0;
    u8 f4, f5, f6, f7, f8, f9;
    u8 padA[0xE];
    u32 f18;
    u8 pad1C[8];
    u32 f24;
    u32 f28;
};

void sub_08214588(struct Obj *);
void sub_08214374(struct Obj *, u32);

void sub_082144A4(struct Obj *o, u32 b, u32 c)
{
    if (o->f4 == 0) {
        o->f0 = c;
        o->f5 = 1;
        o->f18 = 0;
        o->f8 = 0x40;
        o->f9 = 0x40;
        o->f6 = 0;
        o->f7 = 2;
        sub_08214588(o);
        {
            u32 t = (u32)-(c & 0x80) >> 31;
            o->f24 = 0;
            o->f28 = 0;
            sub_08214374(o, t);
        }
    }
}
