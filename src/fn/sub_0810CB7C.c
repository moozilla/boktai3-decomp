// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* RegistryF246_ResetInstance: Clears the singleton pointer */
#define RegistryF246_ResetInstance sub_0810CB7C

void sub_0810CB7C(void)
{
    gLinkPanel = 0;
}
