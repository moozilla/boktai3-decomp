#include "global.h"
#include "scene_actor_control_phase.h"

#define SceneActorPhase_TryBeginSpecialMotion sub_080B0AE8
void Vorn_UpdateScriptedAnimation(struct SceneActorRegistryResetView *);
static inline u8 IsUnavailable(void *input)
{
    struct SceneActorRegistryResetView *p = input;
    if (p) {
        u32 mask = 0x40;
        if (p->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (p->flags2D0 & mask)
            return 1;
        if (p->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 HasFlags(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline u32 MaskedFlags(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2D4 & mask;
}
static inline u8 TestContext(u8 *base, u32 mask, u16 **out)
{
    u16 *field = (u16 *)(base + 0x6D4);
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}
void SceneActorPhase_TryBeginSpecialMotion(struct SceneActorRegistryResetView *p)
{
    u8 *base = (u8 *)p;

    if (!IsUnavailable(base) && *(s16 *)(base + 0x682) == 1 && p->field2B5 != 0x29 &&
        !MaskedFlags(p, 0xE3BF)) {
        u16 *flags;
        if (!TestContext(base, 0xC, &flags) && HasFlags(flags, 2)) {
            u32 state = 0x18;
            u8 *field = &p->state2A8;
            u32 zero = 0;
            *field = zero;
            p->phase2A9 = zero;
            p->behavior2AA = state;
            p->counter2AC = zero;
            {
                u8 *ready = &p->field2B1;
                u32 one = 1;
                void *control;
                u32 *motionFlags;
                *ready = one;
                control = p->savedTarget;
                p->field2B2 = one;
                p->field2B0 = zero;
                p->field2B5 = zero;
                p->target = control;
                p->field2C8 = zero;
                motionFlags = &p->flags114;
                {
                    s32 mask = -2;
                    *motionFlags &= mask;
                }
                {
                    u32 duration;
                    u32 *slot = &p->word104;
                    u32 callback = (u32)Vorn_UpdateScriptedAnimation;
                    duration = 0x29;
                    *slot = callback;
                    p->word108 = zero;
                    p->word10C = zero;
                    p->word110 = duration;
                    *motionFlags = one;
                }
            }
        }
    }
}
