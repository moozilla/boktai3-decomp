#include "global.h"

struct S { u8 pad[0x18]; u32 f18; u8 pad2[0x30-0x1c]; void (*f30)(void); s16 f34; };
void sub_081C35EC(u32, u32, u32);
void sub_081C4E48(void);

void sub_081C4D58(struct S *p)
{
    sub_081C35EC(p->f18, 0, 0x5a);
    p->f30 = sub_081C4E48;
    p->f34 = 0;
}
