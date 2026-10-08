#include "global.h"

s32 sub_081DAB00(u8 *);

s32 sub_081DAC1C(u8 *p, s32 a, u32 n) {
    if (n > 3) {
        return -1;
    }
    *(u16 *)(p + 0x38) = n;
    return sub_081DAB00(p);
}
