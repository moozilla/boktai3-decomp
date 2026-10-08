#include "global.h"

extern u32 *gUnk_020001A0;

void sub_08072BA8(void) {
    u32 *p = gUnk_020001A0;
    if (p) {
        p[7] = 0; p[8] = 0; p[6] = 0; p[10] = 0;
    }
}
