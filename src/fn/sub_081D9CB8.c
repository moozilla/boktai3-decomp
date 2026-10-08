#include "global.h"

u32 sub_081D9CB8(u8 *p) {
    if (*(s16 *)(p + 0x68) <= 0) {
        return 2;
    }
    *(u32 *)(p + 0x8C) |= 1;
    return 0;
}
