#include "global.h"
struct E { u8 f0[0xB3D]; u8 a; u8 f1[7]; u8 b; };
s32 sub_0818505C(struct E *, s32);
void sub_081856D8(struct E *p)
{
    if (p->a == 0) {
        if (sub_0818505C(p, 7) != 0)
            p->a = 0xff;
    }
}
