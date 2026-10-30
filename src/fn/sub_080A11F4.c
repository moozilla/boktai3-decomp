#include "global.h"
#include "scene_actor_reset.h"

s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
static inline u8 TakeEntry(struct SceneActorRegistryResetView *s)
{
    if (s->field2B1) {
        s->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline u32 TestFlags(struct SceneActorRegistryResetView *s, u32 mask)
{
    return s->flags2D4 & mask;
}
static inline void SceneActorControl_ResetReturnState(struct SceneActorRegistryResetView *s)
{
    u32 one = 1;
    u8 *state = &s->state2A8;
    u32 zero = 0;
    *state = zero;
    s->phase2A9 = zero;
    s->behavior2AA = one;
    s->counter2AC = zero;
    s->field2B1 = one;
}
void SceneActorControl_UpdateHeightReturn(struct SceneActorRegistryResetView *s)
{
    s32 delta;
    u16 *height;
    if (TakeEntry(s)) {
        void *fn = s->entryTarget228;
        u32 one = 1;
        u8 *taken = &s->field2B2;
        u32 zero = 0;
        *taken = one;
        s->field2B0 = zero;
        s->field2B5 = one;
        s->target = fn;
        s->field2C8 = zero;
        {
            u32 *flags = &s->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        s->word104 = zero;
        s->body.direction += 0x80;
    }
    Callback_InvokeActor(s, s->contactQuery.opaque);
    if (TestFlags(s, 0xA02F)) {
        s->field2C2 = 0x40;
        height = &s->field2C2;
    } else {
        u16 value = s->field2C2;
        height = &s->field2C2;
        if (!value)
            *height = 0x40;
    }
    if (!s->drawableKind)
        Anim_SetDurationScale(((u8 *)s->drawable) + 0x48, *height);
    else
        Anim_SetFrameDurationScale(((u8 *)s->drawable) + 0x20, *height);
    {
        s32 y;
        s32 offset;
        delta = s->groundHeight2BE;
        y = s->body.position.y;
        offset = -280;
        y += offset;
        delta -= y;
    }
    if (delta < 0)
        delta = -delta;
    if ((delta <= 16) && (((s32)s->counter2AC) > 80)) {
        SceneActorControl_ResetReturnState(s);
    } else {
        ++s->counter2AC;
    }
}
