#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 sub_080E87C4(u8 *);
void sub_080E87B8(u8 *);
void Heap_Free(u8 *);

void sub_080E986C(void)
{
    u8 *p = Heap_Alloc(0x664);
    if (p != 0) {
        Memory_Clear(p, 0x664);
        if (sub_080E87C4(p) < 0) {
            sub_080E87B8(p);
            Heap_Free(p);
        }
    }
}
