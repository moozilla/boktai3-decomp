#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 Octopus_Init(u8 *);
void sub_080EC364(u8 *);
void Heap_Free(u8 *);

void Octopus_Create(void)
{
    u8 *p = Heap_Alloc(0x63c);
    if (p != 0) {
        Memory_Clear(p, 0x63c);
        if (Octopus_Init(p) < 0) {
            sub_080EC364(p);
            Heap_Free(p);
        }
    }
}
