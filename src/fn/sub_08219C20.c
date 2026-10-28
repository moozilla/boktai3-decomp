#include "global.h"
#include "heap.h"


void Heap_Initialize(void)
{
    gHeapHead.prev = 0;
    gHeapHead.next = 0;
    gHeapHead.size = (0x0201F400 - (u32)&gHeapHead) | 0x80000000;
}
