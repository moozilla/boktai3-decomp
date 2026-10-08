#include "global.h"

u32 sub_08045D28(u8 *p, u32 m) {
    return *(u16 *)(p + 4) & m;
}
