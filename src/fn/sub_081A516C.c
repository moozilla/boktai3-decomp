#include "global.h"

struct T { u32 a[3]; };
extern const struct T gUnk_0824F77C;
void sub_0824923C(void *, u32);
struct A { u8 f[0x138]; u16 idx; };

void sub_081A516C(struct A *p)
{
    struct T t = gUnk_0824F77C;
    sub_0824923C(p, t.a[p->idx]);
}
