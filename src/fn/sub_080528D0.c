#include "global.h"
struct O {
    u8 b0[0x18];
    u8 b18;
    u8 pad19[0x50 - 0x19];
    u32 flags50;
    u8 pad54[3];
    u8 b57;
    u8 pad58[0x6e - 0x58];
    s16 s6e;
    u8 pad70[0x7c - 0x70];
    u8 b7c;
    u8 pad7d[0x98 - 0x7d];
    u16 x98;
    s16 h9a;
    u8 pad9c[0xa0 - 0x9c];
    u32 wa0;
    u8 pada4[0x1c0 - 0xa4];
    u16 h1c0;
    u8 pad1c2;
    u8 b1c3;
    u16 h1c4;
    u16 h1c6;
    u8 e1c8[8][0x34];
    u8 b368;
    u8 pad369[3];
    void *p36c;
    u8 pad370[4];
    void *p374;
    void *p378;
    void *p37c;
    u8 pad380[2];
    u16 t382;
};
void sub_08052628(struct O *, void (*)(struct O *));
void sub_08052418(struct O *);
void sub_08052A40(struct O *);
void sub_08020D68(void *, u32);
void sub_080528D0(struct O *o)
{
    o->flags50 &= ~1;
    if (o->s6e < o->h9a + 22)
        o->s6e += 2;
    if (o->h1c0 != 0) {
        o->h1c0--;
        sub_08052418(o);
    } else {
        o->wa0 &= ~1;
        sub_08052628(o, sub_08052A40);
        sub_08020D68(&o->b18, 1);
    }
}
