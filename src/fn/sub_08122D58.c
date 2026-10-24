#include "global.h"
struct ScheduledEffect {
    u8 pad0[0x17A];
    u16 slotFlags; /* first of four slots, 0x144 bytes apart */
    u8 pad17C[0x54C - 0x17C];
    u32 active;
};
extern struct ScheduledEffect *gUnk_02000140;
void SceneScheduledEffect_LoadResource(struct ScheduledEffect *);
// confidence=low: registers the scheduled effect singleton, loads it and marks its four slots.
#ifdef NONMATCHING
// fuzzy: 85.7%
// The ROM loads 0xFFFF into r0 and copies it to r5; this loads r5 directly.
s32 sub_08122D58(struct ScheduledEffect *p)
{
    s32 i;
    gUnk_02000140 = p;
    SceneScheduledEffect_LoadResource(p);
    {
        u32 m = 0xFFFF;
        for (i = 0; i < 4; i++) {
            u16 *f = &(&p->slotFlags)[i * 0xA2];
            u32 v = *f;
            v |= m;
            *f = v;
        }
    }
    p->active = 0;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08122D58);
#endif
