#include "global.h"
#include "ram.h"
struct EffectManager {
    u8 unknown0[0x3c];
    s32 activeCount;
};
struct EffectEntry {
    u32 flags;
    u8 unknown4[0xe8];
    u8 shadow[0x6e];
    u8 motionCount;
    u8 unknown15B[3];
    u16 delay;
    u8 pad[0x8];
    void *motions[1];
};
void MotionEntry_SetEnabled(void *, u32);
void ScaledShadow_Disable(void *);
void EffectEntry_Release(struct EffectEntry *e)
{
    struct EffectManager *m = gEffectPoolManager;
    if (e) {
        s32 i;
        m->activeCount--;
        for (i = 0; i < e->motionCount; i++)
            MotionEntry_SetEnabled(e->motions[i], 1);
        e->delay = 0;
        e->flags |= 1;
        ScaledShadow_Disable(e->shadow);
    }
}
