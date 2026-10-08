#include "global.h"

extern u8 gUnk_08605C28[];

void sub_08083550(u8 *p) {
    *(u8 **)(p + 0x284) = gUnk_08605C28;
}
