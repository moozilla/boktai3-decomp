#include "global.h"

struct S { u8 f[7]; u8 v; };
void sub_081BDC78(struct S *);

void sub_081BDD74(struct S *p, u8 v)
{
    p->v = v;
    sub_081BDC78(p);
}
