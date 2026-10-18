#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Bee_Init(void *);
void Bee_Destroy(void *);

void Bee_Create(void)
{
    void *m = Heap_Alloc(0x490);
    if (m != 0) {
        Memory_Clear(m, 0x490);
        if (Bee_Init(m) < 0) {
            Bee_Destroy(m);
            Heap_Free(m);
        }
    }
}
