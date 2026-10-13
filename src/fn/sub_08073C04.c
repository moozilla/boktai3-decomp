#include "global.h"

extern u8 *gUnk_020004A8;
void Heap_Free(u32 *);

void sub_08073C04(void)
{
    u32 *n = *(u32 **)(gUnk_020004A8 + 0x280);
    while (n) {
        u32 *next = (u32 *)*n;
        Heap_Free(n);
        n = next;
    }
}
