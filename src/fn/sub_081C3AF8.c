#include "global.h"

struct S { u8 pad[0xf8]; u32 f8; };

void sub_081C3AF8(struct S *p)
{
    p->f8++;
}
