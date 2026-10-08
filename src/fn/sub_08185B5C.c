#include "global.h"
struct S { u8 f[0xB3D]; u8 st; u8 g[0xB40 - 0xB3E]; };
void sub_081850B4(void *, s32);
void sub_08185B5C(struct S *p)
{
    u8 *q = &p->st;
    switch (*q) {
    case 0:
        sub_081850B4(p, 4);
        *q = 1;
        break;
    case 1:
        if (*(u8 *)((u8 *)p + 0xB40) == 0xff)
            *q = 0xff;
        break;
    }
}
