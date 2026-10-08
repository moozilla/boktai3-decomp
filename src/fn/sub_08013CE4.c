#include "global.h"

extern u8 *gUnk_02000470;

u32 sub_08013CE4(u8 *p) {
    gUnk_02000470 = p;
    *(u32 *)(p + 0x18) = 0;
    return 0;
}
