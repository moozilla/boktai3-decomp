#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 Mimic_Init(u8 *);
void Mimic_ReleaseResources(u8 *);
void Heap_Free(u8 *);

void Mimic_Create(void)
{
    u8 *p = Heap_Alloc(0x664);
    if (p != 0) {
        Memory_Clear(p, 0x664);
        if (Mimic_Init(p) < 0) {
            Mimic_ReleaseResources(p);
            Heap_Free(p);
        }
    }
}
