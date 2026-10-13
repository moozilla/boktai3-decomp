#include "global.h"

void rfu_clearAllSlot(void);
void sub_080340AC(void *);
u32 sub_0803530C(void *p)
{
    rfu_clearAllSlot();
    sub_080340AC(p);
}
