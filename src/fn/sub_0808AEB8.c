#include "global.h"

void sub_0808AEB8(u8 *p) {
    u8 *q = *(u8 **)(p + 0x3d0);
    u16 v = *(u16 *)(q + 0x63c);
    if (v != 0) *(u16 *)(q + 0x63c) = v - 1;
}
