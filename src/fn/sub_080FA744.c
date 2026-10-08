#include "global.h"

struct S080FA744 { u8 filler[0x284]; const u8 *p; };
extern const u8 gUnk_08606444[];

void sub_080FA744(struct S080FA744 *s)
{
    s->p = gUnk_08606444;
}
