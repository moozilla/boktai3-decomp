#include "global.h"
struct Pool {
    u8 pad[0x20];
    u8 *slots[32];
    u32 mask;
};
void *Heap_Alloc(s32);
void Memory_Clear(void *, s32);
static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}
/* Takes the first free one of the pool's 32 slots: a cleared 0x128-byte block
   recorded at +0x20 with its index at +0x104; NULL when full. */
u8 *sub_08105A44(struct Pool *pool)
{
    s32 i;
    for (i = 0; i < 32; i++) {
        if (!Tst(&pool->mask, i)) {
            u8 *p = Heap_Alloc(0x128);
            Memory_Clear(p, 0x128);
            pool->mask |= 1 << i;
            *(u16 *)(p + 0x104) = i;
            pool->slots[i] = p;
            return p;
        }
    }
    return 0;
}
