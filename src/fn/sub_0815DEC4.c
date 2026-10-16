#include "global.h"

struct P {
    u8 f00[0x3A7];
    u8 c;
    u8 f3a8[0x3A8 - 0x3A8];
};

s32 Collider_Refresh(void *);

void sub_0815DEC4(struct P *p)
{
    u8 *q = &p->c;
    if (*q != 0) {
        Collider_Refresh((u8 *)p + 0x3A8);
        *q = *q - 1;
    }
}
