// COMPILER: old_agbcc
#include "global.h"
/* Script_ClearActorRegistry: Clears the first registry callback to make the table appear empty */

extern u32 gUnk_08603300[];

void Script_ClearActorRegistry(void)
{
    gUnk_08603300[1] = 0;
}
