#include "global.h"

u32 sub_08225B20(u8 *p, u32 v) {
    *(u32 *)(p + 0x24) = v;
    return 1;
}
