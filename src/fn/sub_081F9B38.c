#include "global.h"
extern const u32 gUnk_08613F34[];
struct S { u8 pad[0xAE]; u8 b; };
void sub_0824923C(void *, u32);
void sub_081F9B38(struct S *p)
{
    sub_0824923C(p, gUnk_08613F34[p->b]);
}
