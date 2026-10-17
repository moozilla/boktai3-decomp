#include "global.h"

struct Pool {
    u8 pad[0x38];
    u8 *slots[12];
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
/* Takes the first free one of the pool's twelve slots: a cleared 0xC8-byte
   block recorded at +0x38 with its index at +0xBC; NULL when full. */
u8 *Pool1B8_Allocate(struct Pool *pool)
{
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!Tst(&pool->mask, i)) {
            u8 *p = Heap_Alloc(0xC8);
            Memory_Clear(p, 0xC8);
            pool->mask |= 1 << i;
            *(u16 *)(p + 0xBC) = i;
            pool->slots[i] = p;
            return p;
        }
    }
    return 0;
}
