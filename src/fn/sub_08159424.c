#include "global.h"

struct P08159424 { u8 f0[0x474]; u16 f474; u16 f476; };

void sub_08159424(struct P08159424 *p, u32 a)
{
    p->f474 &= ~a;
    p->f476 = 0;
}
