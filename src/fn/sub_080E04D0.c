// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
// confidence=low: animation 11 is the route-arrival reaction selected by Dog_ResumeRoute.
static inline u8 TakeEntry(struct SceneActorRegistryResetView *a)
{
    if (a->field2B2) {
        a->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline u32 Test(u16 *p, u32 mask)
{
    return *p & mask;
}
void Dog_UpdateArrivalAnimation(struct SceneActorRegistryResetView *a)
{
    u16 saved;
    u16 *flags;
    u16 *scale;
    u16 *originalScale;
    register u32 originalOffset asm("r1");
    if (TakeEntry(a)) {
        SceneActorAnimation_SelectDirectionalEntry(a, 11);
        a->field2B3 = 0;
    }
    originalOffset = 0x2C2;
    originalScale = (u16 *)((u8 *)a + originalOffset);
    saved = *originalScale;
    {
        u32 mask = 0x80;
        flags = &a->flags2D4;
        if (Test(flags, mask))
            *originalScale = Div(0x40, 3);
    }

    if (Test(flags, 0xA02F)) {
        u16 *p = &a->field2C2;
        *p = 0x40;
        scale = p;
    } else {
        u16 *p = &a->field2C2;
        u16 value = *p;
        scale = p;
        if (!value)
            *scale = 0x40;
    }
    if (!a->drawableKind)
        Anim_SetDurationScale((u8 *)a->drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)a->drawable + 0x20, *scale);
    a->field2C2 = saved;
    if (a->field2B3)
        a->field2B0 = 1;
}
