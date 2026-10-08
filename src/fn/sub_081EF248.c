#include "global.h"
void sub_081EE130(void);
struct S {
    u8 f0[0x90]; u32 c90; u8 f1[14]; u8 a2; u8 f2; u32 a4; u8 f3[7];
    u8 af; u8 b0; u8 b1; u8 f4[0x12]; u32 c4; u8 f5[0x38]; void (*fn)(void);
};
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

void sub_081EF248(struct S *p) {
    u8 r = chk(p);
    if (r == 1) {
        void (*f)(void) = sub_081EE130;
        p->fn = f;
        p->c90 = 0;
        p->b1 = r;
        p->a2 = 0;
        p->a4 = 0;
    }
    p->c4++;
}
