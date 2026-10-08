#include "global.h"
u8 *sub_08037DC0(void);
void sub_080396D8(void) {
    u8 *p = sub_08037DC0();
    if (p) p[0x2D] = 1;
}
