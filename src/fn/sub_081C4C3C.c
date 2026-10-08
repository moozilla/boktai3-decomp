#include "global.h"

struct S { u8 pad[0x30]; void (*f30)(void); s16 f34; };
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_081C4EC0(void);

void sub_081C4C3C(struct S *p)
{
    sub_082279A8(1, 6, 4, 4, 4, 0xFFFF, 0);
    p->f30 = sub_081C4EC0;
    p->f34 = 0;
}
