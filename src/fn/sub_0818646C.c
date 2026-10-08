#include "global.h"
struct E { u8 f0[0xB3D]; u8 a; u8 f1[7]; u8 b; };
void sub_08184FE8(struct E *, s32);
void sub_0818646C(struct E *p)
{
    sub_08184FE8(p, 0);
    p->a = 0xff;
}
