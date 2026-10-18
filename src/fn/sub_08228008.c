#include "global.h"
extern u32 gUnk_0201F400;
u32 Heap_IsEndGuardValid(void)
{
    if (gUnk_0201F400 == 0x516B92EE)
        return 1;
    return 0;
}
