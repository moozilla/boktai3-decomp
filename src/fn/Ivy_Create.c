#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 Ivy_Init(u8 *);
void Ivy_Destroy(u8 *);
void Heap_Free(u8 *);

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
