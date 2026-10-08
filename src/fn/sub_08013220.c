#include "global.h"

extern const u32 gUnk_08604B84[];
void sub_08249240(u32 a, u8 *p, u32 f);

u32 sub_08013220(u32 a, u8 *p) {
    sub_08249240(a, p, gUnk_08604B84[p[0x9E]]);
    *(u32 *)(p + 0xA4) += 1;
    return 0;
}
