#include "global.h"

void sub_08018D84(u8 *p) {
    if (p) {
        *(u32 *)(p + 0x1C) = *(u32 *)(p + 0x18);
    }
}
