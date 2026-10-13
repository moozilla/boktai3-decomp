#include "global.h"
void Trigger_ConsumePending(void *, u32);
struct S {
    u8 f0[0xaa];
    u8 aa;
    u8 f1[5];
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};
static inline u8 chk(struct S *p)
{
    u8 *q = &p->b0;
    if (*q) {
        *q = 0;
        q--;
        *q = 0;
        return 1;
    }
    return 0;
}
void sub_08023638(struct S *p)
{
    u8 r = chk(p);
    if (r == 1) {
        p->aa = r;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    }
    p->c4++;
}
