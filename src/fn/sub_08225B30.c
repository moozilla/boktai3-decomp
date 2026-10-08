#include "global.h"

u32 sub_08225B30(u8 *p, u32 v) {
    *(u32 *)(p + 0x30) = v;
    return 1;
}
