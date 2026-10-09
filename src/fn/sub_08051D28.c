#include "global.h"
void sub_08051CB4(void);
void sub_0822B2F8(s32);
struct Obj {
    u8 pad0[0x18];
    u32 f18;
    u8 pad1[0x44];
    u32 f60;
    u8 pad2[2];
    u8 f66;
    u8 f67;
    u8 f68;
    u8 f69;
    u8 pad3[0x40];
    u16 timer;
};
void sub_08051D28(struct Obj *p)
{
    p->f18 |= 1;
    p->f60 &= ~1;
    p->timer++;
    if (p->timer > 0x3f) {
        p->f60 &= ~2;
        p->f68 = 0x40;
        p->f69 = 0x40;
        p->timer = 0;
        *(void (**)(void))((u8 *)p + 0xb4) = sub_08051CB4;
        sub_0822B2F8(0x32f);
    } else {
        p->f60 |= 2;
        p->f68 = p->timer;
        p->f69 = p->timer;
        p->f66 = p->timer << 2;
    }
}
