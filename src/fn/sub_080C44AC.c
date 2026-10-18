#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Skeleton_Init(void *);
void Skeleton_Destroy(void *);

void Skeleton_Create(void)
{
    void *m = Heap_Alloc(0x74c);
    if (m != 0) {
        Memory_Clear(m, 0x74c);
        if (Skeleton_Init(m) < 0) {
            Skeleton_Destroy(m);
            Heap_Free(m);
        }
    }
}
