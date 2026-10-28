// COMPILER: old_agbcc
#include "global.h"
#include "render.h"
/* Anim_SetCurrentListHead: Sets the animation list head for the current list index. */
#define Anim_SetCurrentListHead sub_0821435C

void sub_0821435C(void *v)
{
    gMainSpriteListHeads[gRenderListIndex] = v;
}
