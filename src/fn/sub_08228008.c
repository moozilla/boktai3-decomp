#include "global.h"
#include "ram.h"

u32 Heap_IsEndGuardValid(void)
{
    u32 current;
    HEAP_READ_END_GUARD(current);
    if (current == HEAP_END_GUARD_VALUE)
        return 1;
    return 0;
}
