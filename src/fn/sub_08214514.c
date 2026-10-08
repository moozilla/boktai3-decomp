#include "global.h"

struct Obj {
    u8 pad[4];
    u8 f4;
    u8 pad5[5];
    u8 fA;
};

void sub_082143A0(struct Obj *, u32);

void sub_08214514(struct Obj *o)
{
    if (o->f4 != 0)
        sub_082143A0(o, o->fA);
}
