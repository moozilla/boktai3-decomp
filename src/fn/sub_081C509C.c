#include "global.h"

struct S { u8 pad[0xd4]; void (*fd4)(void); u32 fd8; };
void sub_081C5164(void);

void sub_081C509C(struct S *p)
{
    void (*f)(void) = sub_081C5164;
    p->fd4 = f;
    p->fd8 = 0;
}
