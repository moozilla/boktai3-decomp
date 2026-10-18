// COMPILER: old_agbcc
#include "global.h"
/* Anim_SetCurrentListHead: Sets the animation list head for the current list index. */
#define Anim_SetCurrentListHead sub_0821435C

extern void *gUnk_030042C8[];
extern u32 gUnk_03004290;

void sub_0821435C(void *v)
{
    gUnk_030042C8[gUnk_03004290] = v;
}
