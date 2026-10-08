#include "global.h"
struct P { u8 f0[0xae]; u8 i; u8 g[0x383 - 0xaf]; u8 b; };
extern const u32 gUnk_08611DAC[];
void sub_0824923C(void *, u32);
void sub_0819E408(void *);
void sub_0819E5EC(struct P *p)
{
    p->b = 0;
    sub_0824923C(p, gUnk_08611DAC[p->i]);
    sub_0819E408(p);
}
