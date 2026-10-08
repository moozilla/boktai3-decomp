#include "global.h"

extern u8 gUnk_08605DFC[];

void sub_08099A48(u8 *p) {
    *(u8 **)(p + 0x29c) = gUnk_08605DFC;
}
