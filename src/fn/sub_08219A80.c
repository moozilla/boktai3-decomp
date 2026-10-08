#include "global.h"

struct Obj { u8 pad[0x3A]; u16 f3A; u8 pad3C[0xC]; u8 *f48; };
extern u8 *gUnk_030042E4;

void sub_08219A80(struct Obj *o, u16 v)
{
    s32 t;
    o->f3A = v;
    t = o->f3A;
    o->f48 = gUnk_030042E4 + (t << 5);
}
