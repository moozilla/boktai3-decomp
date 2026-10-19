#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Ghost_Init(void *);
void Ghost_Destroy(void *);

void Ghost_Create(void)
{
    void *m = Heap_Alloc(0x62c);
    if (m != 0) {
        Memory_Clear(m, 0x62c);
        if (Ghost_Init(m) < 0) {
            Ghost_Destroy(m);
            Heap_Free(m);
        }
    }
}
