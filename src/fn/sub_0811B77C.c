// COMPILER: old_agbcc
#include "global.h"
/* RegistryE7BC_ClearFlags: Clears the requested bits in a flag word near the registry manager helpers; ownership is tentative */
#define RegistryE7BC_ClearFlags sub_0811B77C

void sub_0811B77C(u32 *p, u32 bits)
{
    *p &= ~bits;
}
