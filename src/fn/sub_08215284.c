#include "global.h"

struct Obj { u8 pad[6]; u16 f6; u8 pad8[4]; u32 fC; };
extern u16 gUnk_030042E0;
extern u8 *gUnk_030042E4;

void sub_08215284(struct Obj *o, s32 i)
{
    if (i < gUnk_030042E0) {
        o->f6 = i;
        o->fC = (u32)gUnk_030042E4 + (o->f6 << 5);
    }
}
