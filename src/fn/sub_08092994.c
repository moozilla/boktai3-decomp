#include "global.h"

extern u8 gUnk_08605D64[];

void sub_08092994(u8 *p) {
    *(u8 **)(p + 0x284) = gUnk_08605D64;
}
