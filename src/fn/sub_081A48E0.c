#include "global.h"
#define MotionEntry_CreateWithDelta sub_081A48E0
#define gMotionEntryManager         gUnk_02000240
#define MotionEntryManager_Create Hresvelgr_CreateEffectPool
#define MotionEntry_Create          sub_081A4100
struct MotionPosition {
    s16 x, y, z, pad;
};
struct MotionEntry {
    u8 unknown0[0x44];
    struct MotionPosition delta;
};
extern void *gUnk_02000240;
void *Hresvelgr_CreateEffectPool(void);
struct MotionEntry *sub_081A4100(u32, u32, u32, u32);
void *sub_081A48E0(u32 a, struct MotionPosition *delta, u32 b, u32 c)
{
    struct MotionEntry *entry;
    void *result;
    if (!gUnk_02000240)
        gUnk_02000240 = Hresvelgr_CreateEffectPool();
    entry = sub_081A4100(a, b, 1, c);
    if (entry) {
        entry->delta = *delta;
        result = &entry->unknown0;
    } else
        result = 0;
    return result;
}
