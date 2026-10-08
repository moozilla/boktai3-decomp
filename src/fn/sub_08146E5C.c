#include "global.h"

struct P08146E5C {
    u8 f0[0x1D0]; u32 f1D0;
    u8 f1D4[0x433 - 0x1D4]; u8 f433;
    u8 f434[0x4E6 - 0x434]; u16 f4E6; u16 f4E8; u16 f4EA;
    u8 f4EC[0xAC8 - 0x4EC]; u8 fAC8;
};
void sub_08159594(void *);

void sub_08146E5C(struct P08146E5C *p)
{
    p->f1D0 |= 1;
    p->f4E6 = 0;
    p->f4E8 = 0;
    p->f4EA = 0;
    p->f433 = 0;
    sub_08159594(p);
    p->fAC8 = 0xFF;
}
