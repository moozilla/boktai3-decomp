#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 Golem_Init(u8 *);
void Golem_OnDestroy(u8 *);
void Heap_Free(u8 *);

void Golem_Create(void)
{
    u8 *p = Heap_Alloc(0x6b0);
    if (p != 0) {
        Memory_Clear(p, 0x6b0);
        if (Golem_Init(p) < 0) {
            Golem_OnDestroy(p);
            Heap_Free(p);
        }
    }
}
