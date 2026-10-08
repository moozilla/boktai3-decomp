#include "global.h"
struct E { u8 f0[0xB3D]; u8 a; u8 f1[7]; u8 b; };
void sub_081852F4(struct E *, s32, s32);
void sub_08185F70(struct E *, s32, s32);
void sub_0818639C(struct E *p)
{
    if (p->b != 0) {
        p->b--;
        sub_081852F4(p, 0, 0);
    } else {
        sub_08185F70(p, 0, 0);
    }
}
