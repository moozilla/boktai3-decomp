#include "global.h"

struct S {
    u8 pad[0x34];
    u16 f34;
};
void SceneFade_BeginTargetMove(struct S *);

void RegistryC281_UpdateOpeningDelay(struct S *p)
{
    p->f34++;
    if (p->f34 > 0x3c)
        SceneFade_BeginTargetMove(p);
}
