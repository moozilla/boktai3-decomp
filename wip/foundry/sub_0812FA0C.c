#include "global.h"

struct Obj {
    u8 pad0[0x104];
    u32 w104;
    u8 pad108[0x114 - 0x108];
    u32 flags114;
    u8 pad118[0x21C - 0x118];
    void *fn21C;
    u8 pad220[0x22C - 0x220];
    void *w22C;
    u8 pad230[0x270 - 0x230];
    u32 w270;
    u8 pad274[0x2A8 - 0x274];
    u8 b2A8, b2A9, b2AA, b2AB;
    u32 w2AC;
    u8 b2B0, b2B1, b2B2;
    u8 pad2B3[2];
    u8 b2B5;
    u8 pad2B6[0x2C8 - 0x2B6];
    u16 h2C8;
    u8 pad2CA[0x2CC - 0x2CA];
    u32 flags2CC;
    u8 pad2D0[4];
    u16 h2D4;
};

u32 sub_0812F3CC(struct Obj *);
s32 sub_0824923C(void *, void *);

void sub_0812FA0C(struct Obj *o)
{
    void *v;
    if (o->b2B1) {
        o->b2B1 = 0;
        v = o->w22C;
        o->b2B2 = 1;
        o->b2B0 = 0;
        o->b2B5 = 0;
        o->fn21C = v;
        o->h2C8 = 0;
        o->flags114 &= ~1;
        o->w104 = 0;
        o->flags2CC &= ~0x80;
        o->h2D4 &= ~0x200;
    }
    if (sub_0824923C(o, (void *)o->w270))
        return;
    if (sub_0812F3CC(o))
        return;
}
