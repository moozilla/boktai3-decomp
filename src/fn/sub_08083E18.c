#include "global.h"

u32 sub_08074600(u32, u32);

void sub_08083E18(u8 *p) {
    *(u32 *)(p + 0x308) = sub_08074600(0x12, 0);
}
