#include "global.h"

extern u8 gUnk_08605E68[];

void sub_0809D938(u8 *p) {
    *(u8 **)(p + 0x28c) = gUnk_08605E68;
}
