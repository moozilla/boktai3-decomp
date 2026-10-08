#include "global.h"

struct P { u8 f[0x18]; u32 mask; };
struct E { u8 f[0x61]; u8 a; s8 idx; };
void sub_08219DD8(void *, u32);

void sub_08069CD8(struct P *p, struct E *e)
{
    e->a = 0;
    p->mask &= ~(1 << e->idx);
    e->idx = 0xff;
    sub_08219DD8(e, 0x68);
}
