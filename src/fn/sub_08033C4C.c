// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* DialogueTransition_Init: Clears the scripted dialogue transition singleton pointer to initialize its global state */
#define DialogueTransition_Init sub_08033C4C

void sub_08033C4C(void)
{
    gTransition = 0;
}
