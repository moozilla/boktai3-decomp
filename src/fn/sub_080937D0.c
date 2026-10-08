#include "global.h"

void sub_080937D0(u8 *p) {
    u8 *q = p + 0x42b;
    u8 v = *q;
    if (v != 0) *q = v - 1;
}
