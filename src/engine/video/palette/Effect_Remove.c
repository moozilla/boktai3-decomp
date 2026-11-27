#include "global.h"

struct Obj {
    u8 pad[4];
    u8 f4;
    u8 pad5[0xC];
    u8 f11;
};
void Effect_UnlinkFromList(struct Obj *, u32);

void Effect_Remove(struct Obj *o)
{
    if (o->f4 != 0)
        Effect_UnlinkFromList(o, o->f11);
}
