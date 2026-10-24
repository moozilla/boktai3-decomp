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

static inline void SetCounter(struct SceneActorRegistryResetView *s, u32 value)
{
    s->counter2AC = value;
}

// confidence=low: keeps the actor's animation running until it is back near its ground height,
// then returns control to the behavior state machine.
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
        s->field2B0 = 0;
        s->field2B5 = 1;
        s->target = fn;
        s->field2C8 = 0;
        {
            u32 *flags = &s->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        s->word104 = zero;
        s->direction += 0x80;
    }
    Callback_InvokeActor(s, s->update270);
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
        Anim_SetDurationScale((u8 *)s->drawable + 0x48, *height);
    else
        Anim_SetFrameDurationScale((u8 *)s->drawable + 0x20, *height);
    {
        s32 ground = s->groundHeight2BE;
        s32 y = s->position.y;
        s32 offset = -280;
        y += offset;
        delta = ground - y;
    }
    if (delta < 0)
        delta = -delta;
    if (delta <= 16 && (s32)s->counter2AC > 80) {
        u32 one = 1;
        u8 *state = &s->state2A8;
        {
            u8 k1 = 0;
            *state = k1;
        }
        s->phase2A9 = 0;
        s->behavior2AA = 1;
        SetCounter(s, 0);
        s->field2B1 = one;
    } else {
        ++s->counter2AC;
    }
}
