#include "global.h"

struct S { u8 pad0[0xd8]; u32 d8; u8 pad1[0xf4-0xdc]; void (*f4)(void); u32 f8; };
void sub_081C3A98(void);

void sub_081C39E0(struct S *p)
{
    void (*f)(void) = sub_081C3A98;
    p->f4 = f;
    p->f8 = 0;
    p->d8 = 0;
}
