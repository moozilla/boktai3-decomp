#include "global.h"
#include "engine.h"
struct A {
    u32 f0, f4;
    u8 pad[0xCC];
};
struct B {
    u32 f0, f4;
    u8 pad[0xB0];
};
struct Obj {
    u8 pad0[0x1C];
    u16 f1C, f1E, f20, f22, f24;
    u8 pad26[2];
    struct A as[16];
    u8 pad[0xD68 - 0x28 - 0xD40];
    struct B bs[8];
};
/* Resets the -b values (default 30, 10, 180) and clears sixteen 0xD4-byte and
   eight 0xB8-byte records. */
u32 sub_0819A470(struct Obj *p)
{
    s32 i;
    struct A *a;
    struct B *b;
    p->f1C = 0;
    p->f1E = 0;
    p->f20 = 30;
    p->f22 = 10;
    p->f24 = 180;
    if (Script_SeekToKeyword('b')) {
        p->f20 = Script_GetValue();
        p->f22 = Script_GetValue();
        p->f24 = Script_GetValue();
    }
    {
        u32 zero = 0;
        for (a = p->as, i = 15; i >= 0; a++, i--) {
            a->f0 = zero;
            a->f4 = zero;
        }
    }
    {
        u32 zero = 0;
        for (b = p->bs, i = 7; i >= 0; b++, i--) {
            b->f0 = zero;
            b->f4 = zero;
        }
    }
    return 0;
}
