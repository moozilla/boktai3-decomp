#include "global.h"

struct S { u8 f[0xe34]; u8 a; };
void sub_08040A48(struct S *);
void sub_0811CD08(u8 *);
void sub_080414A0(struct S *);
u32 sub_08042044(struct S *p)
{
    sub_08040A48(p);
    sub_0811CD08(&p->a);
    sub_080414A0(p);
    return 0;
}
