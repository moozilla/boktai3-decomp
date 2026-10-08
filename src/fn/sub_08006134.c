#include "global.h"

extern u8 *gUnk_02000028;

u32 sub_08006134(u8 *p) {
    gUnk_02000028 = p;
    *(u32 *)(p + 0x18) = 0;
    return 0;
}
