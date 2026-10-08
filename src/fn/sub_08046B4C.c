#include "global.h"

u32 sub_08046B4C(u8 *p, u32 n) {
    return *(u32 *)(p + 0x18) & (1 << n);
}
