// COMPILER: old_agbcc
#include "global.h"
/* RegistryA8CB_ClearManager: Clears the singleton manager pointer. */
#define RegistryA8CB_ClearManager sub_0812125C

extern u32 gUnk_0200020C;

void sub_0812125C(void)
{
    gUnk_0200020C = 0;
}
