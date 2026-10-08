#include "global.h"
struct S { u8 p[0x14]; u16 h14; u8 q[10]; };
void sub_081FE92C(struct S *);
void sub_081FE970(struct S *, u32, u32, u32, u32);
void sub_081FE98C(struct S *, u32);
u32 sub_081FE760(struct S *, void *);
u32 sub_081FECD8(void *a)
{
    struct S s;
    sub_081FE92C(&s);
    sub_081FE970(&s, 0, 0, 0, 0);
    sub_081FE98C(&s, 1);
    s.h14 = 1;
    return sub_081FE760(&s, a);
}
