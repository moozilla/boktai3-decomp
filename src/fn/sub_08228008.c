#include "global.h"
#include "ram.h"
u32 Heap_IsEndGuardValid(void)
{
    if (gHeapEndGuard == 0x516B92EE)
        return 1;
    return 0;
}
