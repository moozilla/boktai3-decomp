#include "global.h"
void sub_08220F70(void *, void *);
void sub_0811CCE0(u8 *s)
{
    u8 *p;
    sub_08220F70(s + 0x120, p = s + 0x1e0);
    sub_08220F70(s + 0x180, p);
}
