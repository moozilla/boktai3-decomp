#include "global.h"

extern u8 gUnk_08605DD4[];

void sub_080972AC(u8 *p) {
    *(u8 **)(p + 0x284) = gUnk_08605DD4;
}
