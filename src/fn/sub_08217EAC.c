#include "global.h"

struct Obj { u8 pad[4]; u8 f4; u8 pad5[0xC]; u8 f11; };
void sub_082143F8(struct Obj *, u32);

void sub_08217EAC(struct Obj *o)
{
    if (o->f4 != 0)
        sub_082143F8(o, o->f11);
}
