#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Lun_Init(void *);
void Lun_Destroy(void *);

void Lun_Create(void)
{
    void *m = Heap_Alloc(0x44c);
    if (m != 0) {
        Memory_Clear(m, 0x44c);
        if (Lun_Init(m) < 0) {
            Lun_Destroy(m);
            Heap_Free(m);
        }
    }
}
