#include "global.h"

struct Pal { u8 pad[6]; u16 f6; };
struct Obj {
    u32 f0;
    u8 f4;
    u8 pad5[9];
    u8 fE, fF, f10;
    u8 pad11[3];
    u32 f14;
    u8 pad18[8];
    u32 f20, f24;
};
extern u8 *gUnk_030042E4;
void sub_08217EC4(struct Obj *, s32, s32);
void sub_08217EEC(struct Obj *, struct Pal *, u32);
u32 sub_082172E0(u32, u8 *);

void sub_08217E60(struct Obj *o, struct Pal *p, u32 c)
{
    if (o->f4 == 0) {
        o->f0 = c;
        o->f14 = 0;
        o->fF = 1;
        o->f10 = 0;
        sub_08217EC4(o, -8, -8);
        sub_08217EEC(o, p, 0);
        o->fE = sub_082172E0(p->f6, gUnk_030042E4 + (p->f6 << 5));
        o->f20 = 0;
        o->f24 = 0;
    }
}
