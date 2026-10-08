#include "global.h"

u32 sub_08046870(u8 *p) {
    if (p == 0) {
        return 0;
    }
    return *(u32 *)(p + 0x2C);
}
