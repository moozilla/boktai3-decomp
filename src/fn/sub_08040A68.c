#include "global.h"

void sub_08040A68(u8 *p, u32 n) {
    *(u16 *)(p + 0x20) = 0x58;
    *(u16 *)(p + 0x22) = (n << 4) + 0x23;
}
