#include "global.h"

extern const u8 gUnk_08606214[];

struct S { u8 filler[644]; const u8 *p; };

void sub_080E56D8(struct S *s)
{
    s->p = gUnk_08606214;
}
