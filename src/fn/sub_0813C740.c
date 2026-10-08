#include "global.h"

struct E0813C740 { u8 f0[0xF]; u8 fF; u8 f10[0x34 - 0x10]; };
struct Q0813C740 { u32 f0; u8 f4; u8 f5; u8 f6; u8 f7; struct E0813C740 e[4]; };
u32 sub_08215184(u32);
void sub_08217DE0(void *, u32, u32);
void sub_08217EC4(void *, s32, s32);
void sub_08217ECC(void *, u32);

void sub_0813C740(u8 *p)
{
    struct Q0813C740 *q = (struct Q0813C740 *)(p + 0x9A8);
    s32 i;
    q->f0 = sub_08215184(0x1C1E);
    for (i = 0; i < 4; i++) {
        struct E0813C740 *e = &q->e[i];
        sub_08217DE0(e, q->f0, 1);
        sub_08217EC4(e, -4, -4);
        sub_08217ECC(e, 1);
        e->fF = 2;
    }
    q->f5 = 0;
    q->f6 = 0;
}
