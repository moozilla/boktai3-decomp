#include "global.h"

struct S { u8 f[7]; u8 v; };
void sub_0805E5E4(struct S *);

void sub_0805E7C4(struct S *p, u8 v)
{
    p->v = v;
    sub_0805E5E4(p);
}
