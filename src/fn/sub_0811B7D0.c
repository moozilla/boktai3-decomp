#include "global.h"

extern u32 gUnk_02000204;

void RegistryE7BC_OnManagerDestroy(void)
{
    gUnk_02000204 = 0;
}
