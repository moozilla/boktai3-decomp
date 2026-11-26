// COMPILER: old_agbcc
#include "global.h"
/* Script_ClearActorRegistry: Clears the first registry callback to make the table appear empty */
#define Script_ClearActorRegistry sub_08225834

extern u32 gUnk_08603300[];

void sub_08225834(void)
{
    gUnk_08603300[1] = 0;
}
