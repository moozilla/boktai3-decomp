#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
s32 Ivy_Init(u8 *);
u32 Ivy_Destroy(struct SDestroy *s);
void Heap_Free(void *);

void Ivy_Create(void)
{
    u8 *p = Heap_Alloc(0x640);
    if (p != 0) {
        Memory_Clear(p, 0x640);
        if (Ivy_Init(p) < 0) {
            Ivy_Destroy(p);
            Heap_Free(p);
        }
    }
}
