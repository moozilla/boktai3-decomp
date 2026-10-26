// COMPILER: old_agbcc
#include "global.h"
#include "render.h"
/* Effect_SetCurrentListHead: Sets the effect list head for the current list index. */
#define Effect_SetCurrentListHead sub_08214344

extern void *gUnk_030042D0[];

void sub_08214344(void *v)
{
    gUnk_030042D0[gRenderListIndex] = v;
}
