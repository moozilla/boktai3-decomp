#include "global.h"
#include "ram.h"
#define MotionEntryManager_Create Hresvelgr_CreateEffectPool
struct MotionEntry {
    u8 unknown0[0x54];
    u16 parameter54, parameter56, unknown58, parameter5A;
};
void *Hresvelgr_CreateEffectPool(void);
struct MotionEntry *MotionEntry_Create(u32, u32, u32, u32);
void *MotionEntry_CreateConfigured(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f)
{
    struct MotionEntry *entry;
    void *result;
    if (!gMotionEntryManager)
        gMotionEntryManager = Hresvelgr_CreateEffectPool();
    entry = MotionEntry_Create(a, b, 0, f);
    if (entry) {
        entry->parameter54 = d;
        entry->parameter56 = c;
        entry->parameter5A = e;
        result = &entry->unknown0;
    } else {
        result = 0;
    }
    return result;
}
