#include "global.h"

extern const u32 gUnk_08604B6C[];
void sub_08249240(u32 a, u8 *p, u32 f);

void sub_080125FC(u32 a, u8 *p) {
    sub_08249240(a, p, gUnk_08604B6C[p[1]]);
    *(u32 *)(p + 8) += 1;
}
