#include "global.h"
extern u32 gUnk_0300523C;
void sub_08169BE0(void *);
void Hud_OnUnpausedRefresh(void *p)
{
    if (!(gUnk_0300523C & 1))
        sub_08169BE0(p);
}
