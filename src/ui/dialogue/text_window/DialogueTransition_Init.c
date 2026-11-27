// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* DialogueTransition_Init: Clears the scripted dialogue transition singleton pointer to initialize its global state */

void DialogueTransition_Init(void)
{
    gTransition = 0;
}
