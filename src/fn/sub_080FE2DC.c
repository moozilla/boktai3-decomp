#include "global.h"

extern u8 gUnk_08606468[];

void sub_080FE2DC(u8 *p) {
    *(u8 **)(p + 0x28c) = gUnk_08606468;
}
