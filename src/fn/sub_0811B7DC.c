#include "global.h"

extern u32 gUnk_02000204;

s32 RegistryE7BC_InitManager(u32 a)
{
    gUnk_02000204 = a;
    return 0;
}
