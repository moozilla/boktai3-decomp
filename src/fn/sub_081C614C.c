#include "global.h"

struct S {
    u8 pad[0x370];
    s32 f370;
};
void TimeResult_BeginDigitReveal(struct S *);

void TimeResult_UpdateFadeIn(struct S *p)
{
    p->f370++;
    if (p->f370 > 0x20)
        TimeResult_BeginDigitReveal(p);
}
