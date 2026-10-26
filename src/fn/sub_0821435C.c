// COMPILER: old_agbcc
#include "global.h"
#include "render.h"
/* Anim_SetCurrentListHead: Sets the animation list head for the current list index. */
#define Anim_SetCurrentListHead sub_0821435C

extern void *gUnk_030042C8[];

void sub_0821435C(void *v)
{
    gUnk_030042C8[gRenderListIndex] = v;
}
