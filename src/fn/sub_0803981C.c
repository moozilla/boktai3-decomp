#include "global.h"
u8 *sub_08037DC0(void);
void sub_0803981C(void) {
    u8 *p = sub_08037DC0();
    if (p) p[0xEB] = 0;
}
