#include "global.h"
#include "ram.h"
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
void *Actor_Allocate(int unused, int variant)
{
    if (gSceneActorListManager) {
        int size = 0x384;
        u8 *p = Heap_Alloc(size);
        if (p) {
            Memory_Clear(p, size);
            p[1] = variant;
            return p;
        }
    }
    return 0;
}
