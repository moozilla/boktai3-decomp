#include "global.h"

struct P {
    u8 pad0[0xf];
    u8 f0f;
    u8 f10;
    u8 pad11[0x1b];
    s16 f2c;
    s16 f2e;
    u8 pad30[8];
};
struct M {
    u8 pad0[0x18];
    s16 mode;
    u8 pad1a[0xe2 - 0x0];
    void *res;
    struct P p[12];
    u8 pad[0x3A2 - 0x100 - 12 * 0x38];
};

extern void *AnimData_FindEntry(u32);
extern void sub_08217DE0(struct P *, void *, u32);
extern void sub_08217EC4(struct P *, s32, s32);
extern void sub_08217ECC(struct P *, u32);
extern void sub_08217EEC(struct P *, void *, u32);

void sub_081FE04C(struct M *m)
{
    s32 i;
    s16 z;
    struct P *p;
    void **r;
    *(u16 *)((u8 *)m + 0x3a0) = 0;
    *(u16 *)((u8 *)m + 0x3a2) = 0;
    m->res = AnimData_FindEntry(0x1c1e);
    r = &m->res;
    z = 0;
    p = (struct P *)((u8 *)m + 0x100);
    i = 11;
    do {
        sub_08217DE0(p, *r, 1);
        sub_08217EC4(p, -4, -4);
        if (m->mode == 2) {
            sub_08217EEC(p, *r, 1);
            sub_08217ECC(p, 1);
        } else {
            sub_08217EEC(p, *r, 5);
            sub_08217ECC(p, 1);
        }
        p->f0f = 1;
        p->f10 = 0x14;
        p->f2c = z;
        p->f2e = z;
        p++;
        i--;
    } while (i >= 0);
}
