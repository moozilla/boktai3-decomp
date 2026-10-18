#include "global.h"

void ScenePool_DestroyActorSlot(u8 *, u8 *);
void HashTable_Destroy(u8 *);
void Heap_Free(u32);

u32 ScenePool_DestroyManager(u8 *p)
{
    u8 *e = p + 0x44;
    s32 i = 3;
    u32 z;
    do {
        ScenePool_DestroyActorSlot(p, e);
        e += 0x8b0;
        i--;
    } while (i >= 0);
    z = 0;
    HashTable_Destroy(p + 0x2354);
    Heap_Free(*(u32 *)(p + 0x20));
    *(u32 *)(p + 0x20) = z;
    *(u32 *)0x02000108 = z;
    return 0;
}
