#include "global.h"

extern u8 *gUnk_02000468;

void sub_08003448(u32 v) {
    if (gUnk_02000468) {
        u8 *q = gUnk_02000468 + 0x4C;
        q[3] = v;
    }
}
