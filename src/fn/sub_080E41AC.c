#include "global.h"

extern const u8 gUnk_08606204[];

struct S { u8 filler[648]; const u8 *p; };

void sub_080E41AC(struct S *s)
{
    s->p = gUnk_08606204;
}
