#include "global.h"

struct S { u8 pad[0x34]; u32 f34; u8 pad0[0xd8-0x38]; u32 d8; u8 pad1[0xf4-0xdc]; void (*f4)(void); u32 f8; };
void sub_081C3A20(void);

void sub_081C39BC(struct S *p)
{
    void (*f)(void);
    p->f34 &= ~1;
    f = sub_081C3A20;
    p->f4 = f;
    p->f8 = 0;
    p->d8 = 0;
}
