#include "global.h"

struct Obj { u8 pad[0xC]; u8 fC, fD; };

void sub_08217EC4(struct Obj *o, u8 a, u8 b)
{
    o->fC = a;
    o->fD = b;
}
