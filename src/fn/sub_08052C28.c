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
extern u8 *gUnk_02000580;
void sub_0815B858(u8 *, u32, u32);
s32 sub_0805238C(struct O *);
s32 sub_0804A06C(void);
void sub_0821AD08(void *, u32);
void sub_08052CE4(struct O *);
void sub_08052C28(struct O *o)
{
    u8 *pb = &o->b368;
    u32 v = *pb;
    if (v > 1)
        v -= 2;
    else
        v = 0;
    *pb = v;
    o->s6e = o->h9a + o->b368;
    if (o->b368 == 0) {
        if (gUnk_02000580[0x457] == 0) {
            u8 *pf;
            sub_0815B858(gUnk_02000580, 4, 0);
            pf = &o->b1c3;
            *pf = 0;
            if (sub_0805238C(o)) {
                if (sub_0804A06C())
                    *pf = 1;
            }
            o->flags50 &= ~0x200;
            o->b57 = 3;
            o->wa0 |= 1;
            sub_08052628(o, sub_08052CE4);
            if (o->p36c != 0)
                sub_0821AD08(o->p36c, 0);
        }
    }
}
