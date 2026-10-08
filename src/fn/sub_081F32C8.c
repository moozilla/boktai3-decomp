#include "global.h"

u32 sub_081F32C8(u8 *p) {
    u32 r = 0;
    s32 v = *(s16 *)(p + 0);
    if (v <= 0x423) {
        r = 1;
    }
    if (v > 0xCDA) {
        r = 1;
    }
    v = *(s16 *)(p + 4);
    if (v <= 0x527) {
        r = 1;
    }
    if (v > 0xBD6) {
        r = 1;
    }
    return r;
}
