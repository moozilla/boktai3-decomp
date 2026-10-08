#include "global.h"

extern u8 *gUnk_02000710;

void sub_0806FDBC(u8 a) {
    if (a == 1) {
        *(u32 *)(gUnk_02000710 + 0x868) |= a;
    }
}
