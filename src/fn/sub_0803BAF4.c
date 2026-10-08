#include "global.h"

struct Q { u8 f[0xc]; u16 a; u8 b; };
void sub_0803BAF4(u8 *p)
{
    struct Q *q = (struct Q *)(p + 0x860);
    q->b = 0;
    q->a = 0;
}
