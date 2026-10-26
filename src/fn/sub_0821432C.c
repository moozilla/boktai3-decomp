// COMPILER: old_agbcc
#include "global.h"
#include "render.h"
/* Sprite_SetCurrentListHead: Stores a sprite list head in the currently selected list slot */
#define Sprite_SetCurrentListHead sub_0821432C

extern void *gUnk_030042C0[];

void sub_0821432C(void *v)
{
    gUnk_030042C0[gRenderListIndex] = v;
}
