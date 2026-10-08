#include "global.h"
void sub_08020D68(void *, u32);
struct S { u8 f0[0xaa]; u8 aa; u8 f1[5]; u8 b0; u8 f2[0x13]; u32 c4; };
static inline u8 chk(struct S *p) {
    u8 *q = &p->b0;
    if (*q) {
        *q = 0;
        q--;
        *q = 0;
        return 1;
    }
    return 0;
}
void sub_0802B02C(struct S *p) {
    u8 r = chk(p);
    if (r == 1) {
        p->aa = r;
        sub_08020D68((u8 *)p + 0x118, 1);
    }
    p->c4++;
}
