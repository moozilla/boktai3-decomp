#include "global.h"

extern const u8 gUnk_0860613C[];

struct S { u8 filler[652]; const u8 *p; };

void sub_080CFB98(struct S *s)
{
    s->p = gUnk_0860613C;
}
