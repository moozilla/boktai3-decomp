#include "global.h"

void sub_081FD474(u8 *p, u16 a, u32 b, u8 c) {
    *(u16 *)(p + 4) = a;
    *(u32 *)(p + 0x38) = b;
    *(u8 *)(p + 7) = c;
}
