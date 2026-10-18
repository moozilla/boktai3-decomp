// COMPILER: old_agbcc
#include "global.h"
/* DialogueTransition_Init: Clears the scripted dialogue transition singleton pointer to initialize its global state */
#define DialogueTransition_Init sub_08033C4C

extern u32 gUnk_020000E8;

void sub_08033C4C(void)
{
    gUnk_020000E8 = 0;
}
