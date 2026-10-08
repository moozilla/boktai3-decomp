#include "global.h"

struct S { u8 f[0x70]; u32 a; };
void sub_08020D68(void *, u32);
void sub_08020ADC(struct S *p, void *q)
{
    p->a = 1;
    sub_08020D68(q, 1);
}
