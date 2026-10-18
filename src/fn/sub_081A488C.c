#include "global.h"
#define MotionEntry_CreateConfigured sub_081A488C
#define gMotionEntryManager          gUnk_02000240
#define MotionEntryManager_Create Hresvelgr_CreateEffectPool
#define MotionEntry_Create           sub_081A4100
struct MotionEntry {
    u8 unknown0[0x54];
    u16 parameter54, parameter56, unknown58, parameter5A;
};
extern void *gUnk_02000240;
void *Hresvelgr_CreateEffectPool(void);
struct MotionEntry *sub_081A4100(u32, u32, u32, u32);
void *sub_081A488C(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f)
{
    struct MotionEntry *entry;
    void *result;
    if (!gUnk_02000240)
        gUnk_02000240 = Hresvelgr_CreateEffectPool();
    entry = sub_081A4100(a, b, 0, f);
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
