#include "global.h"
#include "ram.h"
struct ScheduledEffectSlot {
    u8 pad0[0x13E];
    u16 flags;
    u8 pad140[4];
};
struct ScheduledEffect {
    u8 pad0[0x3C];
    struct ScheduledEffectSlot slots[4];
    u32 active;
};
void SceneScheduledEffect_LoadResource(struct ScheduledEffect *);
// confidence=low: registers the scheduled effect singleton, loads it and marks its four slots.
s32 sub_08122D58(struct ScheduledEffect *p)
{
    s32 i;
    gAnimatedBodyPool = p;
    SceneScheduledEffect_LoadResource(p);
    for (i = 0; i < 4; i++) {
        p->slots[i].flags |= 0xFFFF;
    }
    p->active = 0;
    return 0;
}
