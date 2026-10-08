#include "global.h"
struct E { u8 f0[0xB3D]; u8 a; u8 f1[7]; u8 b; };
void sub_081850B4(struct E *, s32);
void sub_08185BCC(struct E *p)
{
    if (p->a == 0) {
        sub_081850B4(p, 4);
        p->a = 1;
    }
}
