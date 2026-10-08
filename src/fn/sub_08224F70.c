#include "global.h"

void sub_08224FA4(void);

void sub_08224F70(u32 a) {
    if (a == 0)
        *(u32 *)0x030053E8 = 0x140;
    else
        sub_08224FA4();
}
