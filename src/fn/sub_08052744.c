#include "global.h"
struct O {
    u8 pad0[0x50];
    u32 flags50;
    u8 pad54[3];
    u8 b57;
    u8 pad58[0x6e - 0x58];
    u16 h6e;
    u8 pad70[0x7c - 0x70];
    u8 b7c;
    u8 pad7d[0x98 - 0x7d];
    u16 x98;
    u16 h9a;
    u8 pad9c[0x1c0 - 0x9c];
    u16 h1c0;
    u16 pad1c2;
    u16 h1c4;
    u16 h1c6;
    u8 e1c8[8][0x34];
    u8 b368;
    u8 pad369[0x382 - 0x369];
    u16 t382;
};
void sub_08052628(struct O *, void (*)(struct O *));
void sub_08215284(void *, u32);
void sub_0822B2F8(u32);
void sub_08052418(struct O *);
void sub_0805228C(void *, void *);
void sub_08052744(struct O *o)
{
    o->b368 = 0x18;
    o->h1c6 = 8;
    o->flags50 &= ~1;
    o->h6e = o->h9a + o->b368;
    sub_08215284(&o->b7c, o->h1c4 + 2);
}
