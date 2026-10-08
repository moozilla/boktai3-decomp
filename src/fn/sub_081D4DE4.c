#include "global.h"

struct S { u8 pad[0x1c]; u32 f1c; u8 pad2[0xa03-0x20]; u8 ba03; };

void sub_081D4DE4(struct S *p, u32 v)
{
    p->f1c = v;
    p->ba03 = 1;
}
