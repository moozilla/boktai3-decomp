#include "global.h"

struct S { u8 pad[0x26]; u16 h26; };

void sub_081D4DE0(struct S *p, u16 v)
{
    p->h26 = v;
}
