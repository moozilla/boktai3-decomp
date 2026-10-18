#include "global.h"

extern const u8 gUnk_08606264[];

struct S {
    u8 filler[0x284];
    const u8 *p;
};

void Mimic_InitBehaviorTable(struct S *s)
{
    s->p = gUnk_08606264;
}
