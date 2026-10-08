#include "global.h"
void sub_081BFD00(void);
void sub_081BDB10(u8 *, void (*)(void));
void sub_081BFCD4(u8 *p)
{
    u16 *c = (u16 *)(p + 0x174E);
    (*c)++;
    if (*c > 0x1f) sub_081BDB10(p, sub_081BFD00);
}
