#include "global.h"

extern const u8 gUnk_0860620C[];

struct S { u8 filler[652]; const u8 *p; };

void sub_080E4458(struct S *s)
{
    s->p = gUnk_0860620C;
}
