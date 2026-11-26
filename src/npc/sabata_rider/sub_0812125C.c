// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* RegistryA8CB_ClearManager: Clears the singleton manager pointer. */
#define RegistryA8CB_ClearManager sub_0812125C

void sub_0812125C(void)
{
    gActorManager = 0;
}
