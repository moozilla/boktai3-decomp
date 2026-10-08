#include "global.h"
struct P { u8 f[0x1c0]; u16 f1c0; u8 f1c2[0x382 - 0x1c2]; u16 f382; };
void sub_08052418(struct P *);
void sub_0822B2F8(u32);
void sub_08052628(struct P *, void (*)(void));
void sub_08052D34(void);
void sub_08052CE4(struct P *p)
{
    u16 *c = &p->f1c0;
    if (*c <= 6) {
        *c += 1;
        sub_08052418(p);
    } else {
        u16 *d = &p->f382;
        u32 v = *d + 1;
        *d = v;
        if ((u16)v > 0x1f) {
            sub_0822B2F8(0x15f);
            sub_08052628(p, sub_08052D34);
        }
    }
}
