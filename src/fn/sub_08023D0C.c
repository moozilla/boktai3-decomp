#include "global.h"
void sub_08020D68(void *, u32);
struct S { u8 f0[0xaa]; u8 aa; u8 f1[4]; u8 af; u8 b0; u8 f2[0x13]; u32 c4; };

static inline u8 chk(struct S *p) {
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void sub_08023D0C(struct S *p) {
    if (chk(p)) { u32 k = 0x27; p->aa = k; }
    if (p->af) sub_08020D68((u8 *)p + 0x118, 1);
    else p->c4++;
}
