#include "global.h"
struct E { u8 f0[0xB3D]; u8 a; u8 f1[7]; u8 b; };
s32 sub_08184FE8(struct E *, s32);
void sub_081859E8(struct E *p)
{
    if (p->a == 0) {
        if (sub_08184FE8(p, 0xe) != 0)
            p->a = 0xff;
    }
}
