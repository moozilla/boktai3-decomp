#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 sub_080E5744(u8 *);
void sub_080E5738(u8 *);
void Heap_Free(u8 *);

void sub_080E6814(void)
{
    u8 *p = Heap_Alloc(0x6e0);
    if (p != 0) {
        Memory_Clear(p, 0x6e0);
        if (sub_080E5744(p) < 0) {
            sub_080E5738(p);
            Heap_Free(p);
        }
    }
}
