#include "global.h"

void sub_081FD530(u8 *p, u32 a) {
    *(u32 *)(p + 0x38) = a;
    *(u16 *)(p + 0x54) = 1;
}
