#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 Axe_Init(u8 *);
void sub_080FE668(u8 *);
void Heap_Free(u8 *);

void Axe_Create(void)
{
    u8 *p = Heap_Alloc(0xf24);
    if (p != 0) {
        Memory_Clear(p, 0xf24);
        if (Axe_Init(p) < 0) {
            sub_080FE668(p);
            Heap_Free(p);
        }
    }
}
