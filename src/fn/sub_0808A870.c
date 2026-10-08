#include "global.h"

extern u8 gUnk_08605CE4[];

void sub_0808A870(u8 *p) {
    *(u8 **)(p + 0x28c) = gUnk_08605CE4;
}
