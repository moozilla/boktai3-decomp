#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Spider_Init(void *);
void sub_080A4AA0(void *);

void Spider_Create(void)
{
    void *m = Heap_Alloc(0x65c);
    if (m != 0) {
        Memory_Clear(m, 0x65c);
        if (Spider_Init(m) < 0) {
            sub_080A4AA0(m);
            Heap_Free(m);
        }
    }
}
