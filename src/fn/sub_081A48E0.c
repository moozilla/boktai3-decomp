#include "global.h"
#include "ram.h"
#define MotionEntryManager_Create Hresvelgr_CreateEffectPool
struct MotionPosition {
    s16 x, y, z, pad;
};
struct MotionEntry {
    u8 unknown0[0x44];
    struct MotionPosition delta;
};
void *Hresvelgr_CreateEffectPool(void);
struct MotionEntry *MotionEntry_Create(u32, u32, u32, u32);
void *MotionEntry_CreateWithDelta(u32 a, struct MotionPosition *delta, u32 b, u32 c)
{
    struct MotionEntry *entry;
    void *result;
    if (!gMotionEntryManager)
        gMotionEntryManager = Hresvelgr_CreateEffectPool();
    entry = MotionEntry_Create(a, b, 1, c);
    if (entry) {
        entry->delta = *delta;
        result = &entry->unknown0;
    } else
        result = 0;
    return result;
}
