#include "global.h"

extern u8 gUnk_08605D18[];

void sub_0808F830(u8 *p) {
    *(u8 **)(p + 0x284) = gUnk_08605D18;
}
