#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Bat_Init(void *);
void Bat_Destroy(void *);

void Bat_Create(void)
{
    void *m = Heap_Alloc(0x43c);
    if (m != 0) {
        Memory_Clear(m, 0x43c);
        if (Bat_Init(m) < 0) {
            Bat_Destroy(m);
            Heap_Free(m);
        }
    }
}
