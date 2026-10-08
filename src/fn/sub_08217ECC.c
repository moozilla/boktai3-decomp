#include "global.h"

struct Obj { u8 pad[0xE]; u8 fE; };
extern u8 *gUnk_030042E4;
u32 sub_082172E0(u32, u8 *);

void sub_08217ECC(struct Obj *o, u32 i)
{
    o->fE = sub_082172E0(i, gUnk_030042E4 + (i << 5));
}
