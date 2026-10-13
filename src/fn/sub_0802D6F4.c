#include "global.h"
extern void *gUnk_02000480;
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
void *sub_0802D6F4(int unused, int variant)
{
    if (gUnk_02000480) {
        int size = 0x384;
        u8 *p = Heap_Alloc(size);
        if (p) {
            Memory_Clear(p, size);
            p[1] = variant;
            return p;
        }
    }
    return 0;
}
