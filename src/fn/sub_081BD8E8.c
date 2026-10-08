#include "global.h"
void sub_081BD300(void);
void sub_081BCA74(u8 *);
void sub_081BC6AC(u8 *);
void sub_081BD8E8(u8 *p)
{
    sub_081BD300();
    if (*(u16 *)(p + 0x906)) sub_081BCA74(p);
    else sub_081BC6AC(p);
}
