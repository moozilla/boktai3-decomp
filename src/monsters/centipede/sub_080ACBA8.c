#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Centipede_Init(void *);
void CentipedeBody_Destroy(void *);

void Centipede_Create(void)
{
    void *m = Heap_Alloc(0x934);
    if (m != 0) {
        Memory_Clear(m, 0x934);
        if (Centipede_Init(m) < 0) {
            CentipedeBody_Destroy(m);
            Heap_Free(m);
        }
    }
}
