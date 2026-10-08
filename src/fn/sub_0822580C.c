#include "global.h"
extern const u32 gUnk_08603300[];

void sub_0822580C(void) {
    vu16 *c = (vu16 *)0x030025F4;
    u32 *p;
    *c = 0;
    p = (u32 *)gUnk_08603300;
    while (p[1] != 0) {
        *c = *c + 1;
        p += 2;
    }
}
