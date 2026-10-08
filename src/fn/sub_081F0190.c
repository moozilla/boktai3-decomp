#include "global.h"
struct S { u8 f0[0xb0]; u8 b0; u8 f1[0x13]; u32 c4; };
void sub_081F0190(struct S *p) {
    u8 *q = &p->b0;
    if (*q) {
        *q = 0;
        q--;
        *q = 0;
    }
    p->c4++;
}
