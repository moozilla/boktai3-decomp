#include "global.h"

void sub_081F21B0(u8 *d, u8 *s) {
    *(u32 *)(d + 0) = *(u32 *)(s + 0);
    *(u32 *)(d + 4) = *(u32 *)(s + 4);
    *(u32 *)(d + 8) = *(u32 *)(s + 8);
}
