#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_080A4AFC(void *);
void sub_080A4AA0(void *);

void sub_080A5B54(void)
{
    void *m = Heap_Alloc(0x65c);
    if (m != 0) {
        Memory_Clear(m, 0x65c);
        if (sub_080A4AFC(m) < 0) {
            sub_080A4AA0(m);
            Heap_Free(m);
        }
    }
}
