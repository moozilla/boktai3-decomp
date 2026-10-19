#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_080ABDCC(void *);
void TentacleBody_Destroy(void *);

void TentacleEnemy_Create(void)
{
    void *m = Heap_Alloc(0x934);
    if (m != 0) {
        Memory_Clear(m, 0x934);
        if (sub_080ABDCC(m) < 0) {
            TentacleBody_Destroy(m);
            Heap_Free(m);
        }
    }
}
