#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
s32 Axe_Init(u8 *);
s32 Axe_ReleaseResources(void *p);
void Heap_Free(u8 *);

void Axe_Create(void)
{
    u8 *p = Heap_Alloc(0xf24);
    if (p != 0) {
        Memory_Clear(p, 0xf24);
        if (Axe_Init(p) < 0) {
            Axe_ReleaseResources(p);
            Heap_Free(p);
        }
    }
}
