#include "global.h"
struct P {
    u8 f[0x9a];
    u16 f9a;
    u16 f9c;
    u8 f9e[0xa8 - 0x9e];
    u32 fa8;
    u32 fac;
};
void sub_0824923C(struct P *, u32);
void Script_Call(u32, u32);
void Task_Kill(struct P *);
s32 sub_08056DBC(struct P *p)
{
    u16 *c;
    u32 v;
    sub_0824923C(p, p->fac);
    c = &p->f9a;
    v = *c + 1;
    *c = v;
    if ((u16)v >= p->f9c) {
        if (p->fa8 != 0)
            Script_Call(p->fa8, 0);
        Task_Kill(p);
    }
}
