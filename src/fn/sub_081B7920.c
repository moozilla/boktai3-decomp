#include "global.h"

struct O { u8 f0[0x10]; u16 h10; u8 f12[0xea]; u16 hfc; u16 hfe; u8 f100[2]; u8 b102; u8 f103[0x11]; void (*cb)(void); };
struct O2 { u8 f0[0xf4]; u16 hf4; };
void sub_081B79F8(void);

void sub_081B7920(struct O *p)
{
    u32 a = 2;
    u32 z = 0;
    u32 b = 2;
    void (*f)(void);
    p->h10 = a;
    p->hfc = p->hfe;
    f = sub_081B79F8;
    p->cb = f;
    ((struct O2 *)p)->hf4 = z;
    p->b102 = b;
}
