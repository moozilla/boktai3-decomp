#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
#define EffectPool_UpdateActiveEntries sub_081A3D00
#define EffectPool_UpdateEntry         sub_081A39E0
struct EffectPoolEntry {
    u8 unknown0[0x158];
    u8 frame;
    u8 unknown159[5];
    u16 state;
    u8 unknown160[0x18];
};
struct EffectPool {
    u8 unknown0[0x48];
    struct EffectPoolEntry entries[3];
};
void sub_081A39E0(struct EffectPool *, struct EffectPoolEntry *);
static inline void Clear(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}
void sub_081A3D00(struct EffectPool *pool)
{
    s32 i;
    struct EffectPoolEntry *entry = pool->entries;
    u32 scratch[3];
    Clear(scratch + 1, scratch);
    i = 2;
    do {
        if (entry->state) {
            EffectPool_UpdateEntry(pool, entry);
            entry->frame++;
        }
        entry++;
    } while (--i >= 0);
}
