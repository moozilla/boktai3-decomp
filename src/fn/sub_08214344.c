// COMPILER: old_agbcc
#include "global.h"
/* Effect_SetCurrentListHead: Sets the effect list head for the current list index. */
#define Effect_SetCurrentListHead sub_08214344

extern void *gUnk_030042D0[];
extern u32 gUnk_03004290;

void sub_08214344(void *v)
{
    gUnk_030042D0[gUnk_03004290] = v;
}
