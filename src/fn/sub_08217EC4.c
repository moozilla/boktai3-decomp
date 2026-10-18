#include "global.h"

struct Obj {
    u8 pad[0xC];
    u8 fC, fD;
};

void Effect_SetOrigin(void *object, s32 a, s32 b)
{
    struct Obj *o = object;
    o->fC = a;
    o->fD = b;
}
