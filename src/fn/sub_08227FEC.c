#include "global.h"
#include "ram.h"

void sub_08227FEC(void)
{
    u32 current;
    HEAP_READ_END_GUARD(current);
    if (current != HEAP_END_GUARD_VALUE)
        gHeapEndGuard = HEAP_END_GUARD_VALUE;
}
