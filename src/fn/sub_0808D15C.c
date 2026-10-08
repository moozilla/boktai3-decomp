#include "global.h"

extern u8 gUnk_08605CF0[];

void sub_0808D15C(u8 *p) {
    *(u8 **)(p + 0x29c) = gUnk_08605CF0;
}
