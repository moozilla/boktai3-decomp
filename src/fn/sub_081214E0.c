#include "global.h"
void sub_08121458(void);
void sub_0821FF24(void *, void *, u32);
void sub_0821FE40(void *);
void sub_081214E0(u32 a, u8 *s)
{
    u8 *p, *q;
    sub_08121458();
    p = s + 0x44;
    q = s + 0x1c;
    sub_0821FF24(p, q, 0);
    sub_0821FE40(p);
}
