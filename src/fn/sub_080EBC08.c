#include "global.h"

extern const u8 gUnk_08606338[];
extern const u8 gUnk_08606320[];
extern const u8 gUnk_0860634C[];

struct P { const u8 *a; u32 pad; };
struct S { u8 filler[0x284]; struct P p[3]; };

void sub_080EBC08(struct S *s)
{
    s->p[0].a = gUnk_08606338;
    s->p[1].a = gUnk_08606320;
    s->p[2].a = gUnk_0860634C;
}
