#include "global.h"
#include "scene_actor_control_phase.h"
#ifdef NONMATCHING
// fuzzy: 91.7%
// foundry draft (worker best 91.7%); what differs is mostly register allocation and instruction order
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
void SceneActorPhase_TryBeginSpecialMotion(struct SceneActorRegistryResetView *p)
{
    u8 *base = (u8 *)p;
    if (!IsUnavailable(base) && *(s16 *)(base + 0x682) == 1 && p->field2B5 != 0x29 &&
        !MaskedFlags(p, 0xE3BF)) {
        u32 mask = 0xC;
        u16 *flags = (u16 *)(base + 0x6D4);
        if (!HasFlags(flags, mask) && HasFlags(flags, 2)) {
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
                    u32 duration = 0x29;
                    p->word104 = (u32)Vorn_UpdateScriptedAnimation;
                    p->word108 = zero;
                    p->word10C = zero;
                    p->word110 = duration;
                    *motionFlags = one;
                }
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorPhase_TryBeginSpecialMotion);
#endif
