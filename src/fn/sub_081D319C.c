#include "global.h"

struct S { u8 pad[0x38]; u32 f38; u32 f3c; u8 pad2[0x3a0-0x40]; void (*f3a0)(void); u32 f3a4; };
void sub_081D2FE8(void);

void sub_081D319C(struct S *p, u32 v)
{
    if (p) {
        void (*f)(void);
        p->f38 = v;
        p->f3c = 1;
        f = sub_081D2FE8;
        p->f3a0 = f;
        p->f3a4 = 0;
    }
}
