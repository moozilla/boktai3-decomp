#include "global.h"

void Heap_Free(u32);

void sub_082223F8(u32 *p)
{
    if (p[1] != 0) {
        Heap_Free(p[1]);
        p[1] = 0;
    }
}
