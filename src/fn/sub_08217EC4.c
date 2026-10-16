#include "global.h"

struct Obj {
    u8 pad[0xC];
    u8 fC, fD;
};

void sub_08217EC4(void *object, s32 a, s32 b)
{
    struct Obj *o = object;
    o->fC = a;
    o->fD = b;
}
