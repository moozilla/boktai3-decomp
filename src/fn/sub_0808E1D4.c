#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Ghoul_Init(void *);
void Ghoul_Destroy(void *);

void Ghoul_Create(void)
{
    void *m = Heap_Alloc(0x678);
    if (m != 0) {
        Memory_Clear(m, 0x678);
        if (Ghoul_Init(m) < 0) {
            Ghoul_Destroy(m);
            Heap_Free(m);
        }
    }
}
