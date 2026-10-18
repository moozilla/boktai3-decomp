// COMPILER: old_agbcc
#include "global.h"
/* RegistryE7BC_SetFlags: Sets the requested bits in a flag word near the registry manager helpers; ownership is tentative */
#define RegistryE7BC_SetFlags sub_0811B774

void sub_0811B774(u32 *p, u32 bits)
{
    *p |= bits;
}
