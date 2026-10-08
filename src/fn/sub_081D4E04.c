#include "global.h"

struct S { u8 pad[0x9ec]; u16 h9ec; u16 h9ee; u16 h9f0; };

void sub_081D4E04(struct S *p, u16 a, u16 b)
{
    p->h9ec = a;
    p->h9ee = b;
    p->h9f0 = 0;
}
