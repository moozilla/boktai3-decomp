#include "global.h"

struct S { u8 pad[0x25]; u8 b25; };

void sub_081D4DFC(struct S *p)
{
    p->b25 = 0;
}
