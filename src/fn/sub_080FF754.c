#include "global.h"

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */
void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */
s32 Axe_Init(void *object);
s32 Axe_ReleaseResources(void *p);
void Heap_Free(void *);

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
