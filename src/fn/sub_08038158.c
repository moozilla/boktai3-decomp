#include "global.h"
u8 *sub_08037DC0(void);
void sub_08038158(void) {
    u8 *p = sub_08037DC0();
    if (p) p[0x34] = 1;
}
