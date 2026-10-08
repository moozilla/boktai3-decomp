#include "global.h"
struct P { u8 f[0xA9A]; u8 v; };
u16 *sub_08163F70(s32, s32, s32);
void sub_0816781C(struct P *p)
{
    u16 *q;
    s32 i;
    if (p->v == 0) {
        q = sub_08163F70(0, 0, 0);
        for (i = 0; i <= 8; i++) {
            *q = (i + 0x90) | (s16)0xD000;
            q++;
        }
        p->v = 1;
    }
}
