#include "global.h"
extern u32 *gUnk_02000458;
u32 Script_RegisterCommandTable(u32 *n)
{
    *n = (u32)gUnk_02000458;
    gUnk_02000458 = n;
    return 0;
}
