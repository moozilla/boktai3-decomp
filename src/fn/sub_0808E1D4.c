#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_0808D1D8(void *);
void Ghoul_Destroy(void *);

void Ghoul_Create(void)
{
    void *m = Heap_Alloc(0x678);
    if (m != 0) {
        Memory_Clear(m, 0x678);
        if (sub_0808D1D8(m) < 0) {
            Ghoul_Destroy(m);
            Heap_Free(m);
        }
    }
}
