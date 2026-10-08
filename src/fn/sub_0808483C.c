#include "global.h"

extern u8 gUnk_08605C5C[];

void sub_0808483C(u8 *p) {
    *(u8 **)(p + 0x28c) = gUnk_08605C5C;
}
