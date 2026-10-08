#include "global.h"

extern const u8 gUnk_0860616C[];

struct S { u8 filler[644]; const u8 *p; };

void sub_080D62BC(struct S *s)
{
    s->p = gUnk_0860616C;
}
