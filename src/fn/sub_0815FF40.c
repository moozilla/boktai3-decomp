#include "global.h"

static inline void andm(u32 *a, u32 m) { *a &= m; }
struct P { u8 f00[0x44]; u32 w44; u8 f48[0x9c - 0x48]; u8 s9c[0x56]; u16 hf2; };
extern u32 gUnk_0300523C;
extern u32 gUnk_030053F4;

void sub_0821FE40(void *);
void sub_0821A0C0(void *);

void sub_0815FF40(struct P *p)
{
    u16 *c;
    p->w44 |= 1;
    c = &p->hf2;
    if (*c == 0) {
        andm(&gUnk_0300523C, ~4);
        gUnk_030053F4 &= ~1;
    }
    (*c)++;
    if (*c <= 5)
        sub_0821FE40(p->s9c);
    else
        sub_0821A0C0(p);
}
