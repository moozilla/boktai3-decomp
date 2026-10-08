#include "global.h"

struct S { u8 pad[0xd4]; void (*fd4)(void); u32 fd8; };

void sub_081C5088(struct S *p)
{
    void (*f)(void) = (void (*)(void))0x081C5121;
    p->fd4 = f;
    p->fd8 = 0;
}
