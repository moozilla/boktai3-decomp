#include "global.h"
extern const u32 gUnk_08613F50[];
struct S { u8 pad[0xAD]; u8 b; };
void sub_0824923C(void *, u32);
void sub_081F9BAC(struct S *p)
{
    sub_0824923C(p, gUnk_08613F50[p->b]);
}
