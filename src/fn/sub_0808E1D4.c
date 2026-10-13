#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_0808D1D8(void *);
void sub_0808D1CC(void *);

void sub_0808E1D4(void)
{
    void *m = Heap_Alloc(0x678);
    if (m != 0) {
        Memory_Clear(m, 0x678);
        if (sub_0808D1D8(m) < 0) {
            sub_0808D1CC(m);
            Heap_Free(m);
        }
    }
}
