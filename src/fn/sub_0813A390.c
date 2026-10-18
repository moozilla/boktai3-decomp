#include "global.h"
struct SpritePairEntry {
    u8 pad0[0x44];
    u8 animData[0x1c];
    u32 spriteFlags;
    u8 pad64[3];
    u8 spriteMode;
    u8 pad68[0x14];
    u32 bounds[2];
    u8 pad84[8];
    u8 anim[0x10];
    void *sequence;
    u8 padA0[0x2e];
    u8 active;
    u8 padCF;
    u16 id;
    u8 padD2[0xa];
};
struct SpritePairPool {
    u8 pad0[0x18];
    struct SpritePairEntry entries[2];
    u32 counter;
};
void Memory_Clear(void *, u32);
u32 PairCallbackPool_MarkNextOfKind(struct SpritePairPool *, s32, u32, u8 *);
u32 SpritePairPool_TestUnavailable(struct SpritePairPool *pool, u32 id, s32 required)
{
    s32 matches = 0;
    s32 available = 0;
    s32 i;
    u32 key;
    u8 seen[2];
    key = id & 0xff;
    Memory_Clear(seen, 2);
    for (i = 0; i < 2; i++) {
        if (!pool->entries[i].active)
            available++;
        else if ((pool->entries[i].id & 0xff) == key && !seen[i]) {
            seen[i] = 1;
            PairCallbackPool_MarkNextOfKind(pool, i + 1, pool->entries[i].id, seen);
            matches++;
        }
    }
    if (matches <= 0 && available >= required)
        return 0;
    return 1;
}
