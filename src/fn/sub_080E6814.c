#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 Dog_Init(u8 *);
void Dog_ReleaseResources(u8 *);
void Heap_Free(u8 *);

void Dog_Create(void)
{
    u8 *p = Heap_Alloc(0x6e0);
    if (p != 0) {
        Memory_Clear(p, 0x6e0);
        if (Dog_Init(p) < 0) {
            Dog_ReleaseResources(p);
            Heap_Free(p);
        }
    }
}
