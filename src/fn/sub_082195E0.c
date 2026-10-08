#include "global.h"

struct Obj { u8 pad[4]; u8 f4; u8 pad5[0x17]; u8 f1C; };
void sub_08214450(struct Obj *, u32);

void sub_082195E0(struct Obj *o)
{
    if (o->f4 != 0)
        sub_08214450(o, o->f1C);
}
