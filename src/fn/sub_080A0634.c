#include "global.h"

extern u32 gUnk_08605EA8[];

u32 sub_080A0634(u8 *p) {
    s32 v = *(s32 *)(p + 0x2ac);
    s32 i = 0;
    if (v > 0x1d) {
        i = 1;
        if (v > 0x31) {
            i = 3;
            if (v <= 0x45) i = 2;
        }
    }
    return gUnk_08605EA8[i];
}
