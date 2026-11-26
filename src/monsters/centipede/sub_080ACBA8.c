#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 TentacleActor_Init(void *);
void TentacleBody_Destroy(void *);

void TentacleEnemy_Create(void)
{
    void *m = Heap_Alloc(0x934);
    if (m != 0) {
        Memory_Clear(m, 0x934);
        if (TentacleActor_Init(m) < 0) {
            TentacleBody_Destroy(m);
            Heap_Free(m);
        }
    }
}
