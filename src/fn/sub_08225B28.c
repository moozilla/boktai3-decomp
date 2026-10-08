#include "global.h"

u32 sub_08225B28(u8 *p, u32 v) {
    *(u32 *)(p + 0x2c) = v;
    return 1;
}
