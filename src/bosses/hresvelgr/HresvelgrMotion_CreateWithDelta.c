#include "global.h"
#include "engine/ram.h"
#include "scene/collision/contact_position.h"
#define MotionEntryManager_Create Hresvelgr_CreateEffectPool
struct MotionEntry {
    u8 unknown0[0x44];
    struct ContactPosition delta;
};
void *Hresvelgr_CreateEffectPool(void);
struct MotionEntry *HresvelgrMotion_Create(u32, u32, u32, u32);
void *HresvelgrMotion_CreateWithDelta(u32 a, struct ContactPosition *delta, u32 b, u32 c)
{
    struct MotionEntry *entry;
    void *result;
    if (!gMotionEntryManager)
        gMotionEntryManager = Hresvelgr_CreateEffectPool();
    entry = HresvelgrMotion_Create(a, b, 1, c);
    if (entry) {
        entry->delta = *delta;
        result = &entry->unknown0;
    } else
        result = 0;
    return result;
}
