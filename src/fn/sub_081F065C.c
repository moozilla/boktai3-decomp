#include "global.h"

void sub_081F05D0(void);
void sub_0815F6F0(u8 *, u8 *, void (*)(void));

void sub_081F065C(u8 *p)
{
    sub_0815F6F0(p + 0x250, p + 0x1FC, sub_081F05D0);
}
